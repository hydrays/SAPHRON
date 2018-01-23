require("pracma")

AA <- read.table('test.world0.particle.dat', header=T)
A <- subset(AA, Iteration==0)
NL <- dim(AA)[1]
N <- dim(A)[1]
L <- NL/N

lambda1 = 1;
lambda2 = 0.25;
lambda3 = 0.25;

for ( i in seq(L) )
  {
      iter <- AA[N*i, 1]
      A <- subset(AA, Iteration==iter)
      zz <- file(paste("test.particle.", iter, ".vtk", sep=''), "w")
      cat(file=zz, "# vtk DataFile Version 3.0\n");
      cat(file=zz, "Sphere\n");
      cat(file=zz, "ASCII\n");
      cat(file=zz, "DATASET UNSTRUCTURED_GRID\n");
      cat(file=zz, "POINTS", N, "float\n");
      for ( k in seq(N) )
      {
          cat(file=zz, A$x[k], A$y[k], A$z[k], "\n");
      }
      cat(file=zz, "\n", "POINT_DATA ", N, "\n");
      cat(file=zz, "TENSORS Q float\n");
      for ( k in seq(N) )
      {
          eigen1 <- c(A$ux[k], A$uy[k], A$uz[k])
          if ( abs(eigen1[1]) > 0.1 )
          {
              eigen2 <- c(-(eigen1[2] + eigen1[3])/eigen1[1], 1, 1)
          }
          else if( abs(eigen1[2]) > 0.1 )
          {
              eigen2 <- c(1, -(eigen1[1] + eigen1[3])/eigen1[2], 1)
          }
          else if( abs(eigen1[3]) > 0.1 )
          {
              eigen2 <- c(1, 1, -(eigen1[1] + eigen1[2])/eigen1[3])
          }
          else
          {
              cat("something wrong\n")
              exit
          }
          eigen2 <- eigen2 / sqrt(sum(eigen2*eigen2))
          eigen3 <- cross(eigen1, eigen2)
          eigen3 <- eigen3 / sqrt(sum(eigen3*eigen3))
          Q = lambda1*(eigen1 %o% eigen1) +
              lambda2*(eigen2 %o% eigen2) +
              lambda3*(eigen3 %o% eigen3)
          cat(file=zz, Q[1,], "\n", Q[2,], "\n", Q[3,], "\n\n");
      }
      close(zz)
  }

    ##   for ( gi = 0; gi<num_point; gi++ ){
    ##     x = sx[gi];
    ##     y = sy[gi];
    ##     z = sz[gi];
    ##     r = sqrt(x*x + y*y);
    ##     if (fabs(x) < 1e-14 && y >= 0)
    ##       t = PI/2;
    ##     else if (fabs(x) < 1e-14 && y < 0)
    ##       t = PI*3/2;
    ##     else if (x > 0 && y >= 0)
    ##       t = atan(y/x);
    ##     else if (x > 0 && y < 0)
    ##       t = 2*PI + atan(y/x);
    ##     else if (x < 0 && y >= 0)
    ##       t = PI + atan(y/x);
    ##     else if (x < 0 && y < 0)
    ##       t = PI + atan(y/x);
    ##     p = z;
	
    ##     q1 = interpolation(Qijk,I,J,K,r,t,p,200);
    ##     q2 = interpolation(Qijk + Point,I,J,K,r,t,p,200);
    ##     q3 = interpolation(Qijk + 2*Point,I,J,K,r,t,p,200);
    ##     q4 = interpolation(Qijk + 3*Point,I,J,K,r,t,p,200);
    ##     q5 = interpolation(Qijk + 4*Point,I,J,K,r,t,p,200);
    ##     tensor[0] = q1;
    ##     tensor[1] = q2;
    ##     tensor[2] = q3;
    ##     tensor[3] = q4;
    ##     tensor[4] = q5;
	
    ##     fprintf(fp1,"%16.15f %16.15f %16.15f \n %16.15f %16.15f %16.15f \n %16.15f %16.15f %16.15f\n\n", q1+1.0/3.0,q2,q3,q2,q4+1.0/3.0,q5,q3,q5,-q1-q4+1.0/3.0);
    ##   }
    ## }
