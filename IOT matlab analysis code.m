%MATLAB analytics code:
clc;
clear all;
close all;
[data,timeStamp]=thingSpeakRead(1918312,ReadKey='Y3APGMWDKI2EREWK',Numpoints=800);
%Histogram plot of X co-rdinate acceleration
figure(1);
histogram(data(:,1));
title('X co-ordinate');
%Histogram plot of Y co-rdinate acceleration
figure(2);
histogram(data(:,2));
title('Y co-ordinate');
%Histogram plot of Z co-rdinate acceleration
figure(3);
histogram(data(:,3));
title('Z co-ordinate');
%% Fall analytics
% It plots the data points (i.e. acceleration of the person).
% Positive acceleration in X is fall is towards right direction.
% Negative acceleration in X is fall is towards left direction.
% Positive acceleration in Y is fall is towards front.
% Negative acceleration in Y is fall is towards back.
% Positive acceleration in Z is fall is upwards.(Practically not possible)
% Negative acceleration in Z is fall is downwards.
figure(4);
for i = 1:800
if(data(i,1)<-0.5 || data(i,1)>0.5)
plot(timeStamp(i),data(i,1),'ro');
hold on;
end
end
title('Plot for X');
figure(5);
for i = 1:800
if(data(i,2)<-0.65 || data(i,2)>0.4)
plot(timeStamp(i),data(i,2),'bo');
hold on;
end
end
title('Plot for Y');
j=1;

figure(6);
for i = 1:800
if(data(i,3)<-0.2 || data(i,3)>2)
plot(timeStamp(i),data(i,3),'k^');
z(j)=data(i,3);
hold on;
j=j+1;
end
end
title('Plot for Z');