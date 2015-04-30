d = read.csv("timing.txt")

range_col = 3
time_col = 4
range = 8
x = d[1:range, range_col]
y1 = matrix(d[1:range^2, time_col], ncol=range)
y2 = matrix(d[(range^2+1):(2*range^2), time_col], ncol=range)
y3 = matrix(d[(2*range^2+1):(3*range^2), time_col], ncol=range)

matplot(x, y3, type="o", pch=16, col=seq(1:range), 
        xlab="Buffer Size", ylab="Runtime (s)", 
        main="Runtime vs. Buffer Size for Varied Number of P-C Jobs (Threads)\ninput 3 ~ 10MB")
legend("topright", legend=x, pch=16, inset=.05, col=seq(1:range))
