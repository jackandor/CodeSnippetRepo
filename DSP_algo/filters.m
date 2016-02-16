clear;
Fs=100000;
N=16384;
bm=67;
bn=168;
Nl = ((((((N * 5 + bn) * 2 + bm) * 5 + bn) * 2 + bm) * 5 + bn) * 2 + bm) * 2 + bm;
n=0:N-1;
nl = 0:Nl-1;
mp=5 * 2 * 5 * 2 * 5 * 2 * 2;
t=n/(Fs/mp);
tl=nl/Fs;

f1=3;
A=1;
x=A*sin(2*pi*f1*tl);
%x=load('data.txt');
load('b2.mat', 'b2');
load('b5.mat', 'b5');
z1=filter(b2, 1, x);
z1=z1(bm+1:2:length(z1)); %50000
z2=filter(b2, 1, z1);
z2=z2(bm+1:2:length(z2)); %25000
z3=filter(b5, 1, z2);
z3=z3(bn+1:5:length(z3)); %5000
z4=filter(b2, 1, z3);
z4=z4(bm+1:2:length(z4)); %2500
z5=filter(b5, 1, z4);
z5=z5(bn+1:5:length(z5)); %500
z6=filter(b2, 1, z5);
z6=z6(bm+1:2:length(z6)); %250
z7=filter(b5, 1, z6);
z7=z7(bn+1:5:length(z6)); %50
x=z7;
figure(1);
plot(t,x);title('origenal');
grid;
y=fft(x,N);
mag=abs(y) / N * 2;
f=(0:length(y)-1)'*(Fs/mp)/length(y);
figure(2);
plot(f(1:N/2),mag(1:N/2));
grid;
clear;