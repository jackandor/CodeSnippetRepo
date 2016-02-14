Fs=5000;
N=16384;
n=0:N-1;
t=n/Fs;

%f=1221;
%A=20;
%x=sin(2*pi*f*t);
x=load('data.txt');
figure(1);
plot(t,x);title('origenal');
grid;
y=fft(x,N);
mag=abs(y) / N * 2;
f=(0:length(y)-1)'*Fs/length(y);
figure(2);
plot(f(1:N/2),mag(1:N/2));
grid;