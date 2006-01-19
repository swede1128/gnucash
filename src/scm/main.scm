;; This program is free software; you can redistribute it and/or    
;; modify it under the terms of the GNU General Public License as   
;; published by the Free Software Foundation; either version 2 of   
;; the License, or (at your option) any later version.              
;;                                                                  
;; This program is distributed in the hope that it will be useful,  
;; but WITHOUT ANY WARRANTY; without even the implied warranty of   
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    
;; GNU General Public License for more details.                     
;;                                                                  
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, contact:
;;
;; Free Software Foundation           Voice:  +1-617-542-5942
;; 51 Franklin Street, Fifth Floor    Fax:    +1-617-542-2652
;; Boston, MA  02110-1301,  USA       gnu@gnu.org

(define-module (gnucash main))

(use-modules (ice-9 slib))

(use-modules (g-wrap gw-wct))
(use-modules (g-wrapped gw-core-utils))
(use-modules (g-wrapped gw-gnc))

;; Load the srfis (eventually, we should see where these are needed
;; and only have the use-modules statements in those files).
(use-modules (srfi srfi-1))
(use-modules (srfi srfi-8))

(use-modules (gnucash gnc-module))
(use-modules (ice-9 slib))
(require 'printf)

;; files we can load from the top-level because they're "well behaved"
;; (these should probably be in modules eventually)
(load-from-path "doc.scm")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Exports

;; from main.scm
(export gnc:version)
(export gnc:debugging?)
(export gnc:warn)
(export gnc:error)
(export gnc:msg)
(export gnc:debug)
(export build-path)
(export gnc:use-module-here!)
(export item-list->hash!)
(export string-join)
(export gnc:backtrace-if-exception)
(export gnc:find-file)
(export gnc:find-localized-file)
(export gnc:main)
(export gnc:safe-strcmp) ;; only used by aging.scm atm...
(export gnc:reload-module)

(re-export hash-fold)
(re-export string-split)

;; from path.scm
(export gnc:current-config-auto)
(export gnc:current-saved-reports)
(export gnc:current-saved-stylesheets)

;; from command-line.scm
(export gnc:*config-path*)
(export gnc:*share-path*)
(export gnc:*doc-path*)
(export gnc:*namespace-regexp*)

;; from doc.scm
(export gnc:find-doc-file)
(export gnc:load-help-topics)

;; from main-window.scm
(export gnc:main-window-save-state)
(export gnc:main-window-properties-cb)

;; from printing/print-check.scm
(export make-print-check-format)
(export gnc:print-check)

;; Get the Makefile.am/configure.in generated variables.
(load-from-path "build-config.scm")

;; Do this stuff very early -- but other than that, don't add any
;; executable code until the end of the file if you can help it.
;; These are needed for a guile 1.3.4 bug
(debug-enable 'debug)
(read-enable 'positions)

(debug-set! maxdepth 100000)
(debug-set! stack    2000000)

;;(use-modules (ice-9 statprof))

;;
;; A flag: is this a development version?  This will flag whether or not
;; to print out various 'development version' strings throughout the code.
;; #t == development version, #f == stable version
;;
;; NOTE: You still need to comment out the message in
;; tip_of_the_day.list.in by hand!
;;
(define gnc:*is-development-version?* #t)

;; A list of things to do when in batch mode after the initial
;; startup.  List items may be strings, in which case they're read and
;; evaluated or procedures, in which case they're just executed.
;; The items will be done in reverse order.

(define gnc:*batch-mode-things-to-do* '())

;; These will be converted to config vars later (see command-line.scm)
(define gnc:*debugging?* (if (getenv "GNC_DEBUG") #t #f))
(define gnc:*develmode* (if (getenv "GNC_DEVEL_MODE") #t #f))

;; Function to get debugging
(define (gnc:debugging?)
  (if (boolean? gnc:*debugging?*)
      gnc:*debugging?*
      (gnc:config-var-value-get gnc:*debugging?*)))

(define (gnc:setup-debugging)
  (if (gnc:debugging?)
      (debug-enable 'backtrace)))

;; various utilities

;; Test for simple-format
(if (not (defined? 'simple-format))
    (begin
      (require 'format)
      (export simple-format)
      (define simple-format format)))

(define gnc:use-guile-module-here!
  ;; FIXME: this should be a temporary fix.  We need to check to see
  ;; if there's a more approved way to do this.  As I recall, there's
  ;; not, but I belive a better way will be added to Guile soon.

  ;; module arg must be something like '(ice-9 slib)
  (cond
   ((or (string=? "1.3" (version))
        (string=? "1.3.4" (version))
	(string=? "1.4" (substring (version) 0 3)))
    (lambda (module)
      (process-use-modules (list module))))
   (else
    (lambda (module)
      (process-use-modules (list (list module)))))))

(define (gnc:safe-strcmp a b)
  (cond
   (if (and a b)
       (cond
        ((string<? a b) -1)
        ((string>? a b) 1)
        (else 0))
       (cond
        (a 1)
        (b -1)
        (else 0)))))

(define (gnc:reload-module name)
  (let ((m (current-module)))
    (load-from-path name)
    (set-current-module m)))

(if (not (defined? 'hash-fold))
    (define (hash-fold proc init table)
      (for-each 
       (lambda (bin)
         (for-each 
          (lambda (elt)
            (set! init (proc (car elt) (cdr elt) init)))
          bin))
       (vector->list table))))

(define (item-list->hash! lst hash
			  getkey getval
			  hashref hashset 
			  list-duplicates?)
  ;; Takes a list of the form (item item item item) and returns a hash
  ;; formed by traversing the list, and getting the key and val from
  ;; each item using the supplied get-key and get-val functions, and
  ;; building a hash table from the result using the given hashref and
  ;; hashset functions.  list-duplicates? determines whether or not in
  ;; the resulting hash, the value for a given key is a list of all
  ;; the values associated with that key in the input or just the
  ;; first one encountered.

  (define (handle-item item)
    (let* ((key (getkey item))
	   (val (getval item))
	   (existing-val (hashref hash key)))

      (if (not list-duplicates?)
	  ;; ignore if not first value.
	  (if (not existing-val) (hashset hash key val))
	  ;; else result is list.
	  (if existing-val
	      (hashset hash key (cons val existing-val))
	      (hashset hash key (list val))))))
  
  (for-each handle-item lst)
  hash)

(define (string-join lst joinstr)
  ;; This should avoid a bunch of unnecessary intermediate string-appends.
  ;; I'm presuming those are more expensive than cons...
  (if (or (not (list? lst)) (null? lst))
      ""
      (apply string-append
             (car lst)
             (let loop ((remaining-elements (cdr lst)))
               (if (null? remaining-elements)
                   '()
                   (cons joinstr (cons (car remaining-elements)
                                       (loop (cdr remaining-elements)))))))))

(define (string-split str char)
  (let ((parts '())
        (first-char #f))
    (let loop ((last-char (string-length str)))
      (set! first-char (string-rindex str char 0 last-char))
      (if first-char 
          (begin 
            (set! parts (cons (substring str (+ 1 first-char) last-char) 
                              parts))
            (loop first-char))
          (set! parts (cons (substring str 0 last-char) parts))))    
    parts))

(define (gnc:flatten tree)
  (let ((result '()))
    (let loop ((remaining-items tree))
      (cond
       ((null? remaining-items) #t)
       ((list? remaining-items)
        (loop (car remaining-items))
        (loop (cdr remaining-items)))
       (else
        (set! result (cons remaining-items result)))))
    (reverse! result)))

(define (gnc:backtrace-if-exception proc . args)
  (define (dumper key . args)
    (let ((stack (make-stack #t dumper)))
      (display-backtrace stack (current-error-port))
      (apply display-error stack (current-error-port) args)
      (throw 'ignore)))
  
  (catch 
      'ignore
    (lambda () 
      (lazy-catch #t 
                  (lambda () (apply proc args))
                  dumper))
    (lambda (key . args)
      #f)))

;;;; Status output functions.

(define (gnc:warn . items)
  (display "gnucash: [W] ")
  (for-each (lambda (i) (write i)) items)
  (newline))

(define (gnc:error . items)
  (display "gnucash: [E] ")
  (for-each (lambda (i) (write i)) items)
  (newline))

(define (gnc:msg . items)
  (display "gnucash: [M] ")
  (for-each (lambda (i) (write i)) items)
  (newline))

(define (gnc:debug . items)
  (if (gnc:debugging?)
      (begin
        (display "gnucash: [D] ")
        (for-each (lambda (i) (write i)) items)
        (newline))))


;; Set up timing functions

(define gnc:*last-time* (gettimeofday))

(define (gnc:timestamp . stuff)
  (let* ((now (gettimeofday))
         (delta (+ (- (car now) (car gnc:*last-time*))
                   (/ (- (cdr now) (cdr gnc:*last-time*)) 1000000))))
    (gnc:msg stuff "-- Elapsed time: " delta "seconds.")
    (set! gnc:*last-time* now)))

(define (build-path . elements)
  (string-join elements "/"))

(define (gnc:find-file file directories)
  "Find file named 'file' anywhere in 'directories'.  'file' must be a
string and 'directories' must be a list of strings."

  (gnc:debug "gnc:find-file looking for " file " in " directories)

  (do ((rest directories (cdr rest))
       (finished? #f)
       (result #f))
      ((or (null? rest) finished?) result)

    (let ((file-name (build-path (car rest) file)))
      (gnc:debug "  checking for " file-name)
      (if (access? file-name F_OK)
          (begin
            (gnc:debug "found file " file-name)
            (set! finished? #t)
            (set! result file-name))))))

(define (gnc:find-localized-file file base-directories)
  ;; Find file in path in base directories, or in any localized subdir
  ;; thereof.

  (define (locale-prefixes)
    ;; Mac OS X. 10.1 and earlier don't have LC_MESSAGES. Fall back to
    ;; LC_ALL for those systems.
    (let* ((locale (or (false-if-exception (setlocale LC_MESSAGES))
		       (setlocale LC_ALL)))
           (strings (cond ((not (string? locale)) '())
                          ((equal? locale "C") '())
                          ((<= (string-length locale) 4) (list locale))
                          (else (list (substring locale 0 2)
                                      (substring locale 0 5)
                                      locale)))))
      (reverse (cons "C" strings))))

  (let loop ((prefixes (locale-prefixes))
             (dirs base-directories))
    (if (null? dirs)
        #f
        (or (gnc:find-file file (map (lambda (prefix)
                                       (build-path (car dirs) prefix))
                                     prefixes))
            (gnc:find-file file (list (car dirs)))
            (loop prefixes (cdr dirs))))))

(define (gnc:startup-pass-1)
  (gnc:debug "starting up (1).")
  (gnc:setup-debugging)

  ;; Now we can load a bunch of files.
  (load-from-path "path.scm")
  (load-from-path "command-line.scm") ;; depends on app-utils (N_, etc.)...

  (gnc:initialize-config-vars) ;; in command-line.scm
  ;; handle unrecognized command line args
  (if (not (gnc:handle-command-line-args))
      (gnc:shutdown 1)))

(define (gnc:startup-pass-2)
  (gnc:debug "starting up (2).")

  ;; SUPER UGLY HACK -- this should go away when I come back for the
  ;; second cleanup pass...
  (let ((original-module (current-module))
        (bootstrap (resolve-module '(gnucash main))))
    
    (define (load-module name vers optional?)
      (let ((str (string-append (_ "Loading modules... ") name)))
	(gnc:update-splash-screen str)
	(if optional?
	    (gnc:module-load-optional name vers)
	    (gnc:module-load name vers))))

    (set-current-module bootstrap)

    ;; files we should be able to load from the top-level because
    ;; they're "well behaved" (these should probably be in modules
    ;; eventually)
    (load-from-path "main-window.scm")  ;; depends on app-utils (N_, etc.)...
    (load-from-path "printing/print-check.scm") ;; depends on simple-obj...
    ;; +jsled - 2002.07.08
    (load-from-path "fin.scm")

    (gnc:update-splash-screen (_ "Checking Finance::Quote..."))
    (gnc:use-guile-module-here! '(gnucash price-quotes))
    (let ((sources (gnc:fq-check-sources)))
      (if (list? sources)
	  (gnc:quote-source-set-fq-installed sources)))

    (set-current-module original-module))

  ;; Load the system configs
  (gnc:update-splash-screen (_ "Loading configs..."))
  (if (not (gnc:load-system-config-if-needed)) ;; from path.scm
      (gnc:shutdown 1))

  ;; Load the user configs
  (gnc:load-user-config-if-needed) ;; from path.scm

  (gnc:report-menu-setup)

  ;; the Welcome to GnuCash "extravaganza" report
  (gnc:add-extension 
   (gnc:make-menu-item 
    (N_ "Welcome Sample Report")
    (N_ "Welcome-to-GnuCash report screen")
    (list gnc:menuname-reports gnc:menuname-utility "")
    (lambda (window)
      (gnc:main-window-open-report (gnc:make-welcome-report) window))))

  (gnc:hook-run-danglers gnc:*startup-hook*)

  (if (gnc:config-var-value-get gnc:*loglevel*)
      (gnc:set-log-level-global (gnc:config-var-value-get gnc:*loglevel*))))


(define (gnc:shutdown exit-status)
  (gnc:debug "Shutdown -- exit-status: " exit-status)

  (cond ((gnc:ui-is-running?)
	 (if (not (gnc:ui-is-terminating?))
             (if (gnc:file-query-save)
                 (begin
                   (gnc:hook-run-danglers gnc:*ui-shutdown-hook*)
                   (gnc:gui-shutdown)))))
        (else
	 (gnc:gui-destroy)
	 (gnc:hook-run-danglers gnc:*shutdown-hook*)
         (gnc:engine-shutdown)
	 (exit exit-status))))

(define (gnc:gui-finish)
  (gnc:debug "UI Shutdown hook.")
  (gnc:file-quit))

(define (gnc:strip-path path)
  (let* ((parts-in (string-split path #\/))
	 (parts-out '()))

    ;; Strip out "." and ".." components
    ;; Strip out // components
    (for-each
     (lambda (part)
       (cond ((string=? part ".") #f)
	     ((string=? part "..") (set! parts-out (cdr parts-out)))
	     ((and (string-null? part) (not (= (length parts-out) 0))) #f)
	     (else (set! parts-out (cons part parts-out)))))
     parts-in)

    ;; Put it back together
    (string-join (reverse parts-out) "/")))

(define (gnc:normalize-path file)
  (let* ((parts-in (string-split file #\/))
	 (parts-out '()))

    ;; Convert to a path based at the root.  If the filename starts
    ;; with a '/' then the first component of the list is a null
    ;; string.  If the path starts with foo:// then the first
    ;; component will contain a ':' and the second will be null.
    (cond ((string-null? (car parts-in))
	   (gnc:strip-path file))
	  ((and (string=? (car parts-in) "file:")
		(string-null? (cadr parts-in)))
	   (gnc:strip-path file))
	  ((and (string-index (car parts-in) #\:)
		(string-null? (cadr parts-in)))
	   file)
	  (else
	   (gnc:strip-path (string-append (getenv "PWD") "/" file))))
  )
)

(define (gnc:account-file-to-load)
  (let ((ok (not (gnc:config-var-value-get gnc:*arg-no-file*)))
        (file (if (pair? gnc:*command-line-remaining*)
                  (car gnc:*command-line-remaining*)
                  (gnc:history-get-last))))
    (and ok (string? file) (gnc:normalize-path file))))

(define (gnc:load-account-file)
  (let ((file (gnc:account-file-to-load)))
      (if file 
	(begin
	  (gnc:update-splash-screen (_ "Loading data..."))
	  (and (not (gnc:file-open-file file))
	       (gnc:hook-run-danglers gnc:*book-opened-hook* #f)))
        (and (gnc:hook-run-danglers gnc:*book-opened-hook* #f)))))

(define (gnc:main)

  (define (handle-batch-mode-item item)
    (let ((old-eval? (or (string=? "1.3" (version))
			 (string=? "1.3.4" (version))
			 (string=? "1.4" (substring (version) 0 3)))))
      (cond
       ((procedure? item) (item))
       ((string? item)
	(call-with-input-string
	 item
	 (lambda (port)
	   (let loop ((next-form (read port)))
	     (if (not (eof-object? next-form))
		 (begin
		   ;; FIXME: is this where we want to eval these?
		   ;; should we perhaps have a (gnucash user)?
		   (if old-eval?
		       (eval next-form)
		       (eval next-form (resolve-module '(gnucash main))))
		   (loop (read port))))))))
       (else
	(display "gnucash: unknown batch-mode item - ignoring.")
	(newline)))))

  ;;  (statprof-reset 0 50000) ;; 20 times/sec
  ;;  (statprof-start)

  ;; Now the fun begins.
  (gnc:startup-pass-1)

  (if (null? gnc:*batch-mode-things-to-do*)
      (begin
        (gnc:hook-add-dangler gnc:*ui-shutdown-hook* gnc:gui-finish)
        ;; We init splash before gui-init, but gui-init will do the
        ;; splash itself.
        (set! gnc:*command-line-remaining*
              (gnc:gui-init-splash gnc:*command-line-remaining*))))
  (gnc:startup-pass-2)

  (if (null? gnc:*batch-mode-things-to-do*)
      ;; We're not in batch mode; we can go ahead and do the normal thing.
      (begin
        ;; Why are we doing this again?
        (gnc:hook-add-dangler gnc:*ui-shutdown-hook* gnc:gui-finish)
        (let* ((init-window-cons-rest
                (gnc:gui-init gnc:*command-line-remaining*))
               (main-window (car init-window-cons-rest)))
          (set! gnc:*command-line-remaining* (cdr init-window-cons-rest))
          (if (and
               (not (gnc:account-file-to-load))
               (not (string? (gnc:history-get-last)))
	       (gnc:gconf-get-bool "dialogs/new_user" "first_startup"))
              (begin
                (gnc:destroy-splash-screen)
                (gnc:new-user-dialog))
              (begin
                (gnc:destroy-splash-screen)
                (gnc:main-window-set-progressbar-window main-window)
                (gnc:load-account-file)
                ))
          ;; no matter how or what we loaded, ensure the main-window
          ;; title is valid...
          (gnc:hook-run-danglers gnc:*ui-post-startup-hook*)
          (gnc:start-ui-event-loop) ;; this won't return until we're exiting
          (gnc:hook-remove-dangler gnc:*ui-shutdown-hook* gnc:gui-finish)))
        
      ;; else: we're in batch mode.  Just do what the user said on the
      ;; command line
      (map handle-batch-mode-item (reverse gnc:*batch-mode-things-to-do*)))

  (gnc:shutdown 0))
