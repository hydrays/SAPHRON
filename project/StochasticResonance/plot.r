##require("pracma")
##require(lattice)

OP <- function(u, v)
{
    ##OP <- mean(cos(2*angles))
    OP <- mean(2*(u/sqrt(u*u + v*v))*(u/sqrt(u*u + v*v)) - 1)
    return(OP)
}


AA <- read.table('out.sys1.world0.particle.dat', header=T)
A <- subset(AA, Iteration==0)
NL <- dim(AA)[1]
N <- dim(A)[1]
L <- NL/N

lambda1 = 1;
lambda2 = 0.25;
lambda3 = 0.25;

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
    padded_figure_index <- sprintf("%05d", figure_index)
    png(paste("config_", padded_figure_index, ".png", sep=''))
    plot(A$x, A$y, main=iter, cex=0.5, xlab="x", ylab="y")
    arrows(A$x-.25*A$ux, A$y-.25*A$uy, A$x+.25*A$ux, A$y+.25*A$uy, length=0.0)
    dev.off()
    S <- OP(A$ux, A$uy)
    SList <- c(SList, S)
    Vsin <- c(Vsin, sin(2.0*pi*iter/T))
    Vcos <- c(Vcos, cos(2.0*pi*iter/T))
}
png("op.png")
plot(SList, type='o')
dev.off()

Q <- sqrt( (SList %*% Vsin)**2 + (SList %*% Vcos)**2 ) 
cat(Q, '\n')
