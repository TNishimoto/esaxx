load_filepath <- as.character(commandArgs(trailingOnly=TRUE)[1])
write_filepath <- paste(load_filepath,"write.csv",sep=".")
threshold <- as.numeric(commandArgs(trailingOnly=TRUE)[2])

max <- 0
g<-file(load_filepath,"r")
while(1){
  line <- readLines(g, n=1)
  max <- max+1
  if(length(line) == 0){
    break
  }
}


f<-file(load_filepath,"r")
header <- readLines(f, n=1)
header.x = unlist(strsplit(header, ","))
header_write_mes <- paste("p-value", header.x[2], header.x[3], header.x[4], header.x[5], header.x[6], header.x[7],sep=",")
write(header_write_mes, write_filepath , append = F)
x <- 0
while(1){
  x <- x + 1
  if(x %% 100 == 0){
   message(x,"/",max,"\r",appendLF=FALSE)
   flush.console()
  }
  
  line <- readLines(f, n=1)
  if(length(line) == 0){
    break
  }else{
    line.x = unlist(strsplit(line, ","))
  }
  ABtest_data <- data.frame(
     maximal_substrings = c("Maximal substring", "Maximal substring", "Others", "Others"),
     result = c("(+) items", "(-) items","(+) items", "(-) items"),
     number = c(as.numeric(line.x[3]), as.numeric(line.x[4]), as.numeric(line.x[5]), as.numeric(line.x[6]))
   )
    cross_data <- xtabs(number ~ ., ABtest_data )
    result <- fisher.test(cross_data)
    if(result$p.value <= threshold){
      
      write_mes <- paste(result$p.value, line.x[2], line.x[3], line.x[4], line.x[5], line.x[6], line.x[7],sep=",")
      #print(line.x[1])
      write(write_mes, write_filepath , append = T)
    }
}
