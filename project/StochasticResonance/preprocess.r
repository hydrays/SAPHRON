require(jsonlite)

sys <- "sys1_mc"
js <- read_json(paste(sys, '.json', sep=''))
Cfile <- file("commands", "w")

id <- 0
for ( temperature in seq(0.5, 20, by=0.5) )
{
    id = id + 1
    cat("\n ***********run********* ", id, "\n")
    out <- paste(sys, '_', id, '.json', sep='')
    FolderName <- paste("case_", sys, "_", as.character(id), sep='')
    dir.create(FolderName)

    js$worlds[[1]]$temperature <- temperature
    ## Copy the seqfile.txt into folder
    write_json(js, paste(FolderName, '/', out, sep=''), pretty = TRUE, auto_unbox = TRUE)
    cmd4 <- paste("cp ../../build/saphron", FolderName)
    system(cmd4)

    runcmd <- paste("cd", FolderName, "; screen -d -m ../../build/saphron ", out, "; cd ..")
    ##cat(paste("sleep ", rtime, "; cd", FolderName, "; PhysModel; cd .."), file="run")
    ##runcmd <- paste("screen -d -m ", FolderName, "/PhysModel", sep='')
    ##runcmd <- paste("bsub ", FolderName, "/run -o output_", i, sep='')	
    ##runcmd <- paste("cd", FolderName, "; pwd; BigCAT; cd ..;")
    writeLines(runcmd, Cfile)

}
close(Cfile)

