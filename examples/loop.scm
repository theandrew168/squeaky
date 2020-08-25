(define (loop n)
  (display n)
  (newline)
  (loop (+ n 1)))

(define (loop-til n)
  (define (iter i n)
    (display i)
    (newline)
    (if (= i n)
        #t
        (iter (+ i 1) n)))
  (iter 0 n))
