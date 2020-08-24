(define (draw-quad! window x y w h)
  (window-draw-line! window (- x (/ w 2)) (- y (/ h 2)) (+ x (/ w 2)) (- y (/ h 2)))
  (window-draw-line! window (- x (/ w 2)) (+ y (/ h 2)) (+ x (/ w 2)) (+ y (/ h 2)))
  (window-draw-line! window (- x (/ w 2)) (- y (/ h 2)) (- x (/ w 2)) (+ y (/ h 2)))
  (window-draw-line! window (+ x (/ w 2)) (- y (/ h 2)) (+ x (/ w 2)) (+ y (/ h 2))))

(define (draw-ball! window x y)
  (draw-quad! window x y 20 20))

(define (draw-platform! window x y)
  (draw-quad! window x y 80 20))

; this would be simpler with 'let'
(define (poll-events w)
  (define (iter w events)
    (set! events (cons (window-event-poll w) events))
    (if (eqv? (car events) '())
        (reverse (cdr events))
        (iter w events)))
  (iter w '()))

(define (quit? events)
  (if (null? events)
      #f
      (if (or (eqv? (window-event-type (car events)) 'event-quit)
              (and (eqv? (window-event-type (car events)) 'event-keyboard)
                   (eqv? (window-event-key (car events)) 'key-escape)))
          #t
          (quit? (cdr events)))))

(define (left? events)
  (if (null? events)
      #f
      (if (and (eqv? (window-event-type (car events)) 'event-keyboard)
               (eqv? (window-event-key (car events)) 'key-left))
          #t
          (left? (cdr events)))))

(define (right? events)
  (if (null? events)
      #f
      (if (and (eqv? (window-event-type (car events)) 'event-keyboard)
               (eqv? (window-event-key (car events)) 'key-right))
          #t
          (right? (cdr events)))))

(define (loop window platform events)
  (gc)
  (if (and (left? events) (> platform 40))
      (set! platform (- platform 10)))
  (if (and (right? events) (< platform 760))
      (set! platform (+ platform 10)))
  (window-clear! window)
  (draw-ball! window 400 500)
  (draw-platform! window platform 550)
  (window-present! window)
  (if (quit? events)
      'done
      (loop window platform (poll-events window))))

(loop (make-window "Breakout!" 801 600) 400 '())
