

x=c(1024, 2048, 4096, 8192, 16384, 32768)

# Skip List
# p = 1/2
sl_y_0=c(21, 21, 24, 25, 27, 30)

# p = 1/e = 0.36788
sl_y_1=c(19, 21, 23, 24, 25, 28)

# p = 1/4
sl_y_2=c(20, 22, 23, 26, 29, 30)

# p = 1/8
sl_y_3=c(25, 37, 29, 33, 42, 37)

# p = 1/16
sl_x_4=c(1024, 2048, 4096, 8192, 16384, 32768, 65536, 
      131072, 262144, 524288, 1048576)
sl_y_4=c(50, 80, 39, 49, 66, 102, 53, 63, 87, 121, 72)

sl_0 <- lm(sl_y_0 ~ log2(x))
sl_1 <- lm(sl_y_1 ~ log2(x))
sl_2 <- lm(sl_y_2 ~ log2(x))
sl_3 <- lm(sl_y_3 ~ log2(x))
sl_4 <- lm(sl_y_4 ~ log2(sl_x_4))

summary(sl_0)
summary(sl_1)
summary(sl_2)
summary(sl_3)
summary(sl_4)

# Scapegoat Tree
# alpha = 0.55
st_y_0=c(18, 20, 22, 24, 26, 28)

# alpha = 0.6
st_y_1=c(18, 20, 23, 25, 26, 29)

# alpha = 0.65
st_y_2=c(19, 21, 23, 26, 27, 30)

# alpha = 0.7
st_y_3=c(21, 22, 24, 27, 30, 31)

# alpha = 0.75
st_y_4=c(24, 26, 27, 30, 32, 35)

st_0 <- lm(st_y_0 ~ log2(x))
st_1 <- lm(st_y_1 ~ log2(x))
st_2 <- lm(st_y_2 ~ log2(x))
st_3 <- lm(st_y_3 ~ log2(x))
st_4 <- lm(st_y_4 ~ log2(x))

summary(st_0)
summary(st_1)
summary(st_2)
summary(st_3)
summary(st_4)
