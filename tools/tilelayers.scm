;; The GIMP -- an image manipulation program
;; Copyright (C) 1995 Spencer Kimball and Peter Mattis
;; 
;; Tile layers
;; Copyright (c) 2000 Peter Wang
;; tjaden@users.sourceforge.net
;;
;; This file is gift-ware.  This file is given to you freely
;; as a gift.  You may use, modify, redistribute, and generally hack
;; it about in any way you like, and you do not have to give anyone
;; anything in return.
;;
;; I do not accept any responsibility for any effects, adverse or
;; otherwise, that this code may have on just about anything that
;; you can think of.  Use it at your own risk.


(define (script-fu-tile-layers image drawable)
  (gimp-undo-push-group-start image)
  (let ((w (car (gimp-image-width image)))
	(h (car (gimp-image-height image)))
	(num (car (gimp-image-get-layers image))))
    (gimp-image-resize image w (* num h) 0 0)
    (letrec ((loop (lambda (i max yoffset)
		     (if (< i max)
			 (begin
			   (gimp-layer-translate 
			    (aref (cadr (gimp-image-get-layers image)) i) 0 yoffset)
			   (loop (+ i 1) max (+ yoffset h)))))))
      (loop 0 num 0)))
  (gimp-undo-push-group-end image)
  (gimp-displays-flush))


(script-fu-register "script-fu-tile-layers"
		    "<Image>/Script-Fu/Utils/Tile Layers"
		    "Tile layers vertically"
		    "Peter Wang"
		    "Peter Wang"
		    "June 2000"
		    "RGB*, GRAY*, INDEXED*"
		    SF-IMAGE "Image" 0
		    SF-DRAWABLE "Drawable" 0)


;; tilelayers.scm ends here
