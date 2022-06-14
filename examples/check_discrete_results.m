%% ZSS PT1
data = readmatrix('../build/examples/PT1-Test.csv');
ct = data(:, 1);
cu = data(:, 2);
cy = data(:, 3);

K = 2;
T = 3;
Ts = 0.01;

s = tf('s');
Fs = K / (T * s + 1);
Fz = c2d(Fs, Ts);

[my1, mt1] = step(Fs);
[my2, mt2] = step(Fz);

close all;
figure
hold on
grid on
plot(mt1,my1)
stairs(mt2, my2)
stairs(ct, cy);
legend("Contious", "Discrete", "C")
title("PT1")


%% ZSS HPF
data = readmatrix('../build/examples/HPF-Test.csv');
ct = data(:, 1);
cu = data(:, 2);
cy = data(:, 3);

T = 1;
Ts = 0.1;

s = tf('s');
Fs = T * s / (T * s + 1);

z = zpk('z', Ts);
Fz = (z - 1) / ( z+ Ts/T - 1);

[my1, mt1] = step(Fs);
[my2, mt2] = step(Fz);

close all;
figure
hold on
grid on
plot(mt1,my1)
stairs(mt2, my2)
stairs(ct, cy);
legend("Contious", "Discrete", "C")
title("HPF")

%% ZSS DI
data = readmatrix('../build/examples/DI-Test.csv');
ct = data(:, 1);
cu = data(:, 2);
cx = data(:, 3);
cy = data(:, 4);

T = 1;
Ts = 0.01;
% 
% s = tf('s');
% Fs = T * s / (T * s + 1);

z = zpk('z', Ts);
Fhpf = (z - 1) / ( z+ Ts/T - 1);
Fhpf = ss(Fhpf);
Fhpf.InputName = {'u'};
Fhpf.OutputName = {'x'};

Fdi = pid(0, 1, 0, 100, Ts);
Fdi = ss(Fdi);
Fdi.InputName = {'x'};
Fdi.OutputName = {'y'};

Fz = connect(Fhpf, Fdi, {'u'}, {'u', 'x', 'y'});
[mv, mt] = step(Fz);
mu = mv(:, 1);
mx = mv(:, 2);
my = mv(:, 3);

close all;
figure
hold on
grid on
stairs(mt, my)
stairs(ct, cy);
legend("Discrete", "C")
title("DI")

%% ZSS DD
data = readmatrix('../build/examples/DD-Test.csv');
ct = data(:, 1);
cu = data(:, 2);
cy = data(:, 3);

T = 1;
Ts = 0.01;

Fdd = pid(0, 0, 2, 0.2, Ts);


[my, mt] = step(Fdd);

close all;
figure
hold on
grid on
stairs(mt, my)
stairs(ct, cy);
legend("Discrete", "C")
title("DD")

%% Schmitt-Quantizer

data = readmatrix('../build/examples/Schmitt-Quantizer-Test.csv');
ct = data(:, 1);
cu = data(:, 2);
cy = data(:, 3);

close all;

figure
hold on
grid on
plot(ct, cu)
plot(ct, cy)
xlabel("t")
ylabel("u, y")
legend("u", "y")
title("Schmitt-Quantizer")

figure
hold on
grid on
plot(cu, cu)
plot(cu, cy)
xlabel("u")
ylabel("y")
legend("uu", "uy")
title("Schmitt-Quantizer")
