(define w (make-window "Asteroids" 800 800))

(define (draw-ship! x y)
  (window-draw-line! w x y (+ x 10) (+ y 30))
  (window-draw-line! w x y (- x 10) (+ y 30))
  (window-draw-line! w x (+ y 20) (+ x 10) (+ y 30))
  (window-draw-line! w x (+ y 20) (- x 10) (+ y 30)))

; test expr is bugged if not present
(do ((e 'event-undefined (window-event-poll w)))
    ((eq? (window-event-type e) 'event-quit) 123)
  (window-clear! w)
  (draw-ship! 200 200)
  (window-present! w))

(display "done")
(newline)
