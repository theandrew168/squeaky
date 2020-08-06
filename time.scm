(define (loop-til x n)
  (if (= x n)
    'done
    (loop-til (+ 1 x) n)))

(define (loop x)
  (display x)
  (newline)
  (loop (+ x 1)))
