# Make sure this file is in your src/ folder
# You should be able to access threads/lab3-stats.csv
# You should be able to access processes/lab3-stats.csv
# Make sure to set the current working directory to this files:
#   Session -> Set Working Directory -> To Source File Location
# 
# Copyright Jonathan Shahen 2018


require(ggplot2)

# Read in Stats data for threads
t_data = read.csv(file="threads/lab3-stats.csv", header=TRUE, sep=",");
names(t_data)[5] = 'avg';
names(t_data)[6] = 'stddev';
t_data['type'] = 't';
# Set up new column called 'testcase' to be string 'B P C'
t_data['testcase'] = paste(t_data$B, t_data$P, t_data$C);

# Read in Stats data for processes
p_data = read.csv(file="processes/lab3-stats.csv", header=TRUE, sep=",");
names(p_data)[5] = 'avg';
names(p_data)[6] = 'stddev';
p_data['type'] = 'p';
# Set up new column called 'testcase' to be string 'B P C'
p_data['testcase'] = paste(p_data$B, p_data$P, p_data$C);

all_data = rbind(p_data, t_data);
all_data['pttestcase'] = paste(all_data$type, all_data$testcase);

# Plot all the 'testcase' lines across with N in the horizontal and average time in the vertical
p = ggplot(p_data, aes(N, avg))+
  geom_line(aes(color = testcase))+
  ggtitle('All Lines (Processes Only)')+
  ylab('Average Time (ms)')+
  xlab('Number of Total Tasks')
ggsave('plot1.png', width=12, height=6);
plot(p)

g = ggplot(t_data[t_data$B==8 & t_data$P==1,], aes(N, avg))+
  geom_line(aes(color = testcase))+
  ggtitle('All Lines Where B == 8 and P == 1 (Threads Only)')+
  ylab('Average Time (ms)')+
  xlab('Number of Total Tasks')
ggsave('plot2.png', width=12, height=6);
plot(g)

h = ggplot(all_data[all_data$B==8 & all_data$P==1,], aes(N, avg))+
  geom_line(aes(color = pttestcase))+
  ggtitle('All Lines Where B == 8 and P == 1 (Processors vs. Threads)')+
  ylab('Average Time (ms)')+
  xlab('Number of Total Tasks')
ggsave('plot3.png', width=12, height=6);
plot(h)

i = ggplot(all_data[all_data$B==8 & all_data$P==1,], aes(N, avg))+
  geom_line(aes(color = testcase))+
  facet_grid(.~type)+
  ggtitle('All Lines Where B == 8 and P == 1 (Processors vs. Threads)')+
  ylab('Average Time (ms)')+
  xlab('Number of Total Tasks')
ggsave('plot4.png', width=12, height=6);
plot(i)

j = ggplot(all_data[all_data$B==8 & all_data$P==1,], aes(N, avg))+
  geom_line(aes(color = testcase))+
  facet_grid(type~.)+
  ggtitle('All Lines Where B == 8 and P == 1 (Processors vs. Threads)')+
  ylab('Average Time (ms)')+
  xlab('Number of Total Tasks')
ggsave('plot5.png', width=12, height=6);
plot(j)

k = ggplot(all_data[all_data$B==8 & all_data$P==1,], aes(N, avg))+
  geom_line(aes(color = testcase, linetype=type))+
  ggtitle('All Lines Where B == 8 and P == 1 (Processors vs. Threads)')+
  ylab('Average Time (ms)')+
  xlab('Number of Total Tasks')
ggsave('plot6.png', width=12, height=6);
plot(k)

