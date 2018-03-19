require(jsonlite)

##sys <- "ll_sys625_lg"
##sys <- "sys_n_625_beta_0.5"
sys <- "sys_n_2500_beta_0.5"
js <- read_json(paste(sys, '.json', sep=''))

C2file <- file("total_commands", "w")

case_id <- 0

##A <- 0.7
cc <- 0.75

for ( A in seq(0.0, 0.9, by=0.05) )
{
    case_id <- case_id + 1
    CfileName <- paste("commands_", as.character(case_id), sep='')
    Cfile <- file(CfileName, "w")
    id <- 0
    for ( temperature in seq(0, 0.6, by=0.03) )
        ##for ( temperature in seq(10, 200, by=10) )
        ##for ( AE in seq(0.1, 2, by=0.1) )
    {
        id = id + 1
        cat("\n ***********run********* ", id, "\n")
        out <- paste(sys, '_', id, '.json', sep='')
        FolderName <- paste("case_", cc, "_", sys, "_", as.character(case_id), "_", as.character(id), sep='')
        dir.create(FolderName)

        js$worlds[[1]]$temperature <- temperature
        js$forcefields$A <- A
        js$forcefields$constraints[[1]]$coefficient = cc
        ##js$forcefields$constraints[[1]]$coefficient <- AE
        ## Copy the seqfile.txt into folder
        write_json(js, paste(FolderName, '/', out, sep=''), pretty = TRUE, auto_unbox = TRUE)
        cmd4 <- paste("cp ../../build/saphron", FolderName)
        system(cmd4)

        ##runcmd <- paste("cd", FolderName, "; screen -d -m ../../../build/saphron ", out, "; cd ..")
        runcmd <- paste("cd", FolderName, "; ../../../build/saphron ", out, "; cd ..;")
        ##cat(paste("sleep ", rtime, "; cd", FolderName, "; PhysModel; cd .."), file="run")
        ##runcmd <- paste("screen -d -m ", FolderName, "/PhysModel", sep='')
        ##runcmd <- paste("bsub ", FolderName, "/run -o output_", i, sep='')	
        ##runcmd <- paste("cd", FolderName, "; pwd; BigCAT; cd ..;")
        writeLines(runcmd, Cfile)
    }
    close(Cfile)
    cmd <- paste("chmod +x", CfileName)
    system(cmd)

    runcmd <- paste("screen -d -m", CfileName)
    writeLines(runcmd, C2file)
}
close(C2file)
