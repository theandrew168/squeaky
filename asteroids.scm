(define (draw-ship! w x y)
  (window-draw-line! w x y (+ x 10) (+ y 30))
  (window-draw-line! w x y (- x 10) (+ y 30))
  (window-draw-line! w x (+ y 20) (+ x 10) (+ y 30))
  (window-draw-line! w x (+ y 20) (- x 10) (+ y 30)))

(define (quit? events)
  (if (null? events)
      #f
      (if (eqv? (window-event-type (car events)) 'event-quit)
          #t
          (quit? (cdr events)))))
      

(define (loop w)
  (window-clear! w)
  (draw-ship! w 100 100)
  (window-present! w)
  (if (quit? (window-event-poll w))
      'done
      (loop w)))

(loop (make-window "Asteroids" 500 500))
