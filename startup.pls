(begin
	(define make-point (lambda (x y) (set-property "object-name" "point" (list x y))))
	(define make-line (lambda (p1 p2) (set-property "object-name" "line" (list p1 p2))))
	(define make-text (lambda (text) (set-property "position" (make-point 0 0) (set-property "object-name" "text" text))))
)