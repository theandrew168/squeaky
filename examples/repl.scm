(define env (interaction-environment))

(define (repl)
  (write-char #\>)
  (write-char #\space)
  (write (eval (read) env))
  (newline)
  (repl))

(repl)
