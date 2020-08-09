(define w (make-window "Asteroids" 800 800))

(define (draw-ship! w x y)
  (window-draw-line! w x y (+ x 10) (+ y 30))
  (window-draw-line! w x y (- x 10) (+ y 30))
  (window-draw-line! w x (+ y 20) (+ x 10) (+ y 30))
  (window-draw-line! w x (+ y 20) (- x 10) (+ y 30)))
