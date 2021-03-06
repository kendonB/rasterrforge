# Author: Robert J. Hijmans
# Date :  June 2008
# Version 1.0
# Licence GPL v3


setMethod('hist', signature(x='Raster'), 
	function(x, layer, maxpixels=100000, plot=TRUE, main, ...) {
		
		if (missing(layer)) {
			y <- 1:nlayers(x)
		} else if (is.character(layer)) {
			y <- match(layer, names(x))
		} else { 
			y <- layer 
		}
		
		y <- unique(as.integer(round(y)))
		y <- stats::na.omit(y)
		y <- y[ y >= 1 & y <= nlayers(x) ]
		nl <- length(y)
		
		if (nl == 0) {
			stop('no layers selected')
		}

		if (missing(main)) {
			main=names(x) 
		}
		
		if (nl > 1)	{
			res <- list()
			if (nl > 16) {
				warning('only the first 16 layers are plotted')
				nl <- 16
				y <- y[1:16]
			}

			nc <- ceiling(sqrt(nl))
			nr <- ceiling(nl / nc)
			mfrow <- graphics::par("mfrow")
			spots <- mfrow[1] * mfrow[2]
			if (spots < nl) {
				graphics::par(mfrow=c(nr, nc))
			}
			for (i in 1:length(y)) {
				res[[i]] = .hist1(raster(x, y[i]), maxpixels=maxpixels, main=main[y[i]], plot=plot, ...) 
			}		

		} else if (nl==1) {
			if (nlayers(x) > 1) {
				x <- x[[y]]
				main <- main[y]
			}
			res <- .hist1(x, maxpixels=maxpixels, main=main, plot=plot, ...) 	
		}		
		if (plot) {
			return(invisible(res))
		} else {
			return(res)
		}
	}
)



.hist1 <- function(x, maxpixels, main, plot, ...){

		if ( inMemory(x) ) {
			v <- getValues(x)
		} else if ( fromDisk(x) ) {
			
			if (ncell(x) <= maxpixels) {
				v <- stats::na.omit(getValues(x))
			} else {

			# TO DO: make a function that does this by block and combines  all data into a single histogram
				v <- sampleRandom(x, maxpixels)
				msg <- paste(round(100 * maxpixels / ncell(x)), "% of the raster cells were used", sep="")
				if (maxpixels > length(v)) {
					msg <- paste(msg, " (of which ", 100 - round(100 * length(v) / maxpixels ), "% were NA)", sep="")
				}
				warning( paste(msg, ". ",length(v)," values used.", sep="") )
			}	
		} else { 
			stop('cannot make a histogram; need data on disk or in memory')
		}	
		
		if (.shortDataType(x) == 'LOG') {
			v <- v * 1
		}
		
		if (plot) {
			hist(v, main=main, plot=plot, ...)  
		} else {
			hist(v, plot=plot, ...)  		
		}
}	



