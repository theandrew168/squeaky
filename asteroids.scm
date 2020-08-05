(define w (make-window "Asteroids" 800 800))

(define (draw-ship! x y)
  (window-draw-line! w x y (+ x 10) (+ y 30))
  (window-draw-line! w x y (- x 10) (+ y 30))
  (window-draw-line! w x (+ y 20) (+ x 10) (+ y 30))
  (window-draw-line! w x (+ y 20) (- x 10) (+ y 30)))

(window-clear! w)
(draw-ship! 200 200)
(window-present! w)