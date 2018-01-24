##require("pracma")
##require(lattice)

sys <- "sys1_mc"

OP <- function(u, v)
{
    ##OP <- mean(cos(2*angles))
    OP <- mean(2*(u/sqrt(u*u + v*v))*(u/sqrt(u*u + v*v)) - 1)
    return(OP)
}

id <- 0
QList <- NULL
for ( rep in seq(40) )
{
    id = id + 1
    cat("\n ***********run********* ", id, "\n")
    out <- paste(sys, '_', id, '.json', sep='')
    FolderName <- paste("case_", sys, "_", as.character(id), sep='')

    AA <- read.table(paste(FolderName, '/out.sys1.world0.particle.dat', sep=''), header=T)
    A <- subset(AA, Iteration==0)
    NL <- dim(AA)[1]
    N <- dim(A)[1]
    L <- NL/N
    T <- 2*1000

    SList <- NULL
    Vsin <- NULL
    Vcos <- NULL
    figure_index <- 0
    for ( i in seq(L) )
    {
        figure_index <- figure_index + 1
        iter <- AA[N*i, 1]
        A <- subset(AA, Iteration==iter)
        ## padded_figure_index <- sprintf("%05d", figure_index)
        ## png(paste("config_", padded_figure_index, ".png", sep=''))
        ## plot(A$x, A$y, main=iter, cex=0.5, xlab="x", ylab="y")
        ## arrows(A$x-.25*A$ux, A$y-.25*A$uy, A$x+.25*A$ux, A$y+.25*A$uy, length=0.0)
        ## dev.off()
        S <- OP(A$ux, A$uy)
        SList <- c(SList, S)
        Vsin <- c(Vsin, sin(2.0*pi*iter/T))
        Vcos <- c(Vcos, cos(2.0*pi*iter/T))
    }
    Q <- sqrt( (SList %*% Vsin)**2 + (SList %*% Vcos)**2 ) 
    cat(Q, '\n')
    QList <- c(QList, Q)
}

png("QvsD.png")
plot(QList, type='o')
dev.off()



