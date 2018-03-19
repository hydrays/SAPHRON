require(jsonlite)
##require("pracma")
require(lattice)

col.l <- colorRampPalette(c('red', 'orange', 'yellow', 'green', 'cyan', 'blue'))

sys <- "sys_n_2500_beta_0.5"
##sys <- "sys_n_625_beta_0.5"

OP <- function(u, v)
{
    ##OP <- mean(cos(2*angles))
    ##OP <- mean(2*(u/sqrt(u*u + v*v))*(u/sqrt(u*u + v*v)) - 1)
    OP <- mean(((u+v)/sqrt(u*u + v*v))**2 - 1)
    return(OP)
}

cc <- 0.5
xmax <- 25.5

outdata <- NULL
case_id <- 0
for ( rep2 in seq(19) )
{
    case_id <- case_id + 1
    
    id <- 0
    QList <- NULL
    DList <- NULL
    for ( rep in seq(21) )
    {
        id = id + 1
        cat("\n ***********run********* ", id, "\n")
        out <- paste(sys, '_', id, '.json', sep='')
        FolderName <- paste("case_", cc, "_", sys, "_", as.character(case_id), "_", as.character(id), sep='')

        AA <- read.table(paste(FolderName, '/out.sys1.world0.particle.dat', sep=''), header=T)
        A <- subset(AA, Iteration==0)
        NL <- dim(AA)[1]
        N <- dim(A)[1]
        L <- NL/N
        T <- 50000

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
            ## S <- OP(A$ux, A$uy)
            
            P <- rbind(subset(A, A$x == 0.5 & A$y == 0.5), subset(A, A$x == 0.5 & A$y == xmax-0.5 ),
                       subset(A, A$x == xmax-0.5 & A$y == 0.5), subset(A, A$x == xmax-0.5 & A$y == xmax-0.5 ))
            ##P <- rbind(subset(A, A$x == 0.5 & A$y == 0.5), subset(A, A$x == xmax - 0.5 & A$y == xmax - 0.5 ))
            ##P <- A[c(25, 597), ]
            S <- OP(P$ux, P$uy)
            
            SList <- c(SList, S)
            Vsin <- c(Vsin, sin(2.0*pi*iter/T))
            Vcos <- c(Vcos, cos(2.0*pi*iter/T))
        }
        js <- read_json(paste(FolderName, '/out.sys1.chkpt.json', sep=''))
        D <- js$worlds[[1]]$temperature
        amp <- js$forcefields$A
        Tn <- js$forcefields$T
        In <- js$observers[[1]]$frequency
        Tt <- js$iterations

        Q <- In*2.0*sqrt( (SList %*% Vsin)**2 + (SList %*% Vcos)**2 )/Tt

        if (id == 1)
        {
            dQ <- 1
            Q0 <- Q
        }
        else
        {
            dQ <- Q/Q0
        }
        
        cat(amp, D, Q, dQ, '\n')
        QList <- c(QList, Q)
        DList <- c(DList, D)

        outdata <- rbind(outdata, c(amp, D, Q, dQ))

        ## pdf(paste("op/op_", FolderName, ".pdf", sep=''), height=4, width=9)
        ## par(mar=c(4,6,4,4))
        ## plot(Vsin[1:100], type='l', xlab=expression(t), ylab=expression(S), cex.lab=1.5, cex.axis=1.5)
        ## lines(SList[1:100], type='o', col='blue', lwd=2, cex=0.75)
        ## dev.off()

    }

    ## png("QvsD.png")
    ## plot(QList, type='o')
    ## dev.off()

    ## write.csv(cbind(QList, DList), file=paste('QvsD_', sys, '.csv' ,sep=''))
}

write.csv(outdata, file=paste("outdata_", FolderName, ".csv", sep=''))

png(paste("SRmap_", FolderName, ".png", sep=''))
p1 <- levelplot(outdata[,3]~outdata[,2]*outdata[,1], asp=1, col.regions=col.l)
print(p1)
dev.off()
