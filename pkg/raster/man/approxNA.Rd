\name{approxNA}

\docType{methods}

\alias{approxNA}
\alias{approxNA,RasterStackBrick-method}

\title{Estimate values for cell values that are \code{NA} by interpolating between layers}

\description{
approxNA uses the \code{stats} function \code{\link{approx}} to estimate values for cells that are \code{NA} by interpolation across layers. Layers are considered equidistant, unless an argument 'z' is used, or \code{\link{getZ}} returns values, in which case these values are used to determine distance between layers.

For estimation based on neighbouring cells see \code{\link{focal}}
}

\usage{
\S4method{approxNA}{RasterStackBrick}(x, filename="", method="linear", yleft, yright,
            rule=1, f=0, ties=mean, z=NULL, NArule=1, ...) 
}

\arguments{
  \item{x}{RasterStack or RasterBrick object}
  \item{filename}{character. Output filename (optional)}
  \item{method}{specifies the interpolation method to be used. Choices are "linear" or "constant" (step function; see the example in \code{\link{approx}}}
  \item{yleft}{the value to be returned before a non-\code{NA} value is encountered. The default is defined by the value of rule given below}
  \item{yright}{the value to be returned after the last non-\code{NA} value is encountered. The default is defined by the value of rule given below}
  \item{rule}{an integer (of length 1 or 2) describing how interpolation is to take place at for the first and last cells (before or after any non-\code{NA} values are encountered). If rule is 1 then NAs are returned for such points and if it is 2, the value at the closest data extreme is used. Use, e.g., \code{rule = 2:1}, if the left and right side extrapolation should differ}
  \item{f}{for method = "constant" a number between 0 and 1 inclusive, indicating a compromise between left- and right-continuous step functions. If y0 and y1 are the values to the left and right of the point then the value is \code{y0*(1-f)+y1*f} so that \code{f = 0)} is right-continuous and \code{f = 1} is left-continuous}
  \item{ties}{Handling of tied 'z' values. Either a function with a single vector argument returning a single number result or the string "ordered"}
  \item{z}{numeric vector to indicate the distance between layers (e.g., time, depth). The default is 1:nlayers(x) }  
  \item{NArule}{single integer used to determine what to do when only a single layer with a non-\code{NA} value is encountered (and linear interpolation is not possible). The default value of 1 indicates that all layers will get this value for that cell; all other values do not change the cell values}  
  \item{...}{additional arguments as for \code{\link{writeRaster}}}
}

\value{
RasterBrick
}

\seealso{ \code{ \link{focal}} } 


\examples{
r <- raster(ncols=5, nrows=5)
r1 <- setValues(r, runif(ncell(r)))
r2 <- setValues(r, runif(ncell(r)))
r3 <- setValues(r, runif(ncell(r)))
r4 <- setValues(r, runif(ncell(r)))
r5 <- setValues(r, NA)
r6 <- setValues(r, runif(ncell(r)))
r1[6:10] <- NA
r2[5:15] <- NA
r3[8:25] <- NA
s <- stack(r1,r2,r3,r4,r5,r6)
s[1:5] <- NA
x1 <- approxNA(s)
x2 <- approxNA(s, rule=2)
x3 <- approxNA(s, rule=2, z=c(1,2,3,5,14,15))

}

\keyword{spatial}

