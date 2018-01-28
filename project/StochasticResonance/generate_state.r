require(jsonlite)

sys <- "lattice_sys5_mc"
js <- read_json(paste(sys, '.json', sep=''))

##n <- 32800
xmax <- 12
ymax <- 12
##x <- runif(n, min=0, max=xmax)
##y <- runif(n, min=0, max=ymax)
cx <- seq(0, xmax, by=0.5)
cy <- seq(0, ymax, by=0.5)
theta <- runif(length(cx)*length(cy), min=0, max=2*pi)
ux <- cos(theta)
uy <- sin(theta)

particles <- NULL

##for ( i in seq(n) )
i <- 0
for ( x in cx )
{
    for ( y in cy )
    {
        ##p <- list(i+1, "GB", c(cx[i], cy[i], 0.0), c(ux[i],uy[i],0.0))
        i <- i+1
        p <- list(i+1, "GB", c(x, y, 0.0), c(ux[i],uy[i],0.0))
        particles[[i]] <- p
    }
}

n <- length(cx)*length(cy)
js$worlds[[1]]$particles <- particles
js$worlds[[1]]$components[[1]][[2]] <- n
js$mpi <- n
js$worlds[[1]]$dimensions[[1]] <- xmax
js$worlds[[1]]$dimensions[[2]] <- ymax


## js$iterations = 10000
## js$forcefields$nonbonded[[1]]$diameters = c(3.0, 3.0)
## js$forcefields$nonbonded[[1]]$dw = 1.0
## js$forcefields$nonbonded[[1]]$epsS = 1.0
## js$forcefields$nonbonded[[1]]$mu = 1.0
## js$forcefields$nonbonded[[1]]$nu = 1.0
## js$forcefields$nonbonded[[1]]$lengths = c(3.0, 3.0)
## js$forcefields$constraints[[1]]$coefficient = 0.0

## js$moves[[1]]$dx = 0.08
## js$moves[[1]]$type = "Translate"
## js$moves[[1]]$weight = 50
## js$moves[[1]]$maxangle <- NULL

## js$worlds[[1]]$periodic$x <- TRUE
## js$worlds[[1]]$periodic$y <- TRUE
## js$worlds[[1]]$periodic$z <- FALSE

write_json(js, 'lattice_sys6_mc.json', pretty = TRUE, auto_unbox=TRUE)

