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

%% ZSS PID
data = readmatrix('../build/examples/PID-Test.csv');
ct = data(:, 1);
cu = data(:, 2);
cy = data(:, 3);

Ts = 0.1;

Fpid = pid(1, 1, 1, 1, Ts);
[my, mt] = step(Fpid);

close all;
figure
hold on
grid on
stairs(mt, my)
stairs(ct, cy);
legend("Discrete", "C")
title("PID")

%% Simulink PID

Ts = 0.1;
sim('pid_sim.slx')

mt = out.tout;
mv = out.mv;
mr = mv(:, 1);
me = mv(:, 2);
mu = mv(:, 3);
my = mv(:, 4);
mm = mv(:, 5);

close all;
figure
hold on
grid on
stairs(mt, mr)
stairs(mt, me)
stairs(mt, mu)
stairs(mt, my)
stairs(mt, mm)
legend("r", "e", "u", "y", "m")
title("Simulink PID")

%% C-PID

data = readmatrix('../build/examples/PID2-Test.csv');
ct = data(:, 1);
cr = data(:, 2);
ce = data(:, 3);
cu = data(:, 4);
cy = data(:, 5);
cm = data(:, 6);

close all;
figure
hold on
grid on
stairs(ct, cr)
stairs(ct, ce)
stairs(ct, cu)
stairs(ct, cy)
stairs(ct, cm)
legend("r", "e", "u", "y", "m")
title("Simulink PID")

%% Simulink vs. C PID v1

Ts = 0.1;
sim('pid_sim.slx')

mt = out.tout;
mv = out.mv;
mr = mv(:, 1);
me = mv(:, 2);
mx = mv(:, 3);
mu = mv(:, 4);
my = mv(:, 5);
mm = mv(:, 6);

close all;
figure
hold on
grid on
stairs(mt, mr)
stairs(mt, me)
stairs(mt, mx)
stairs(mt, mu)
stairs(mt, my)
stairs(mt, mm)
legend("r", "e", "x", "u", "y", "m")
title("Simulink PID")

% C-PID
data = readmatrix('../build/examples/PID2-Test.csv');
ct = data(:, 1);
cr = data(:, 2);
ce = data(:, 3);
cx = data(:, 4);
cu = data(:, 5);
cy = data(:, 6);
cm = data(:, 7);


figure
hold on
grid on
stairs(ct, cr)
stairs(ct, ce)
stairs(ct, cx)
stairs(ct, cu)
stairs(ct, cy)
stairs(ct, cm)
legend("r", "e", "x", "u", "y", "m")
title("C-PID")

close all;

% r
subplot(6, 1, 1)
hold on
grid on
stairs(mt, mr)
stairs(ct, cr)
legend("Matlab", "C")
title("r")

% e
subplot(6, 1, 2)
hold on
grid on
stairs(mt, me)
stairs(ct, ce)
legend("Matlab", "C")
title("e")

% x
subplot(6, 1, 3)
hold on
grid on
stairs(mt, mx)
stairs(ct, cx)
legend("Matlab", "C")
title("x")

% u
subplot(6, 1, 4)
hold on
grid on
stairs(mt, mu)
stairs(ct, cu)
legend("Matlab", "C")
title("u")

% y
subplot(6, 1, 5)
hold on
grid on
stairs(mt, my)
stairs(ct, cy)
legend("Matlab", "C")
title("y")

% m
subplot(6, 1, 6)
hold on
grid on
stairs(mt, mm)
stairs(ct, cm)
legend("Matlab", "C")
title("m")

%% Simulink vs. C PID v2

Ts = 1;
sim('pid_sim.slx')

mt = out.tout;
mv = out.mv;
mr = mv(:, 1);
me = mv(:, 2);
mx = mv(:, 3);
mu = mv(:, 4);
my = mv(:, 5);
mm = mv(:, 6);

close all;
figure
hold on
grid on
stairs(mt, mr)
stairs(mt, me)
stairs(mt, mx)
stairs(mt, mu)
stairs(mt, my)
stairs(mt, mm)
legend("r", "e", "x", "u", "y", "m")
title("Simulink PID")

% C-PID
data = readmatrix('../build/examples/PID2-Test.csv');
ct = data(:, 1);
cr = data(:, 2);
ce = data(:, 3);
cx = data(:, 4);
cu = data(:, 5);
cy = data(:, 6);
cm = data(:, 7);


figure
hold on
grid on
stairs(ct, cr)
stairs(ct, ce)
stairs(ct, cu)
stairs(ct, cy)
stairs(ct, cm)
legend("r", "e", "u", "y", "m")
title("C-PID")

close all;

% r
figure
hold on
grid on
stairs(mt, mr)
stairs(ct, cr)
legend("Matlab", "C")
title("r")

% e
figure
hold on
grid on
stairs(mt, me)
stairs(ct, ce)
legend("Matlab", "C")
title("e")

% x
figure
hold on
grid on
stairs(mt, mx)
stairs(ct, cx)
legend("Matlab", "C")
title("x")

% u
figure
hold on
grid on
stairs(mt, mu)
stairs(ct, cu)
legend("Matlab", "C")
title("u")

% y
figure
hold on
grid on
stairs(mt, my)
stairs(ct, cy)
legend("Matlab", "C")
title("y")

% m
figure
hold on
grid on
stairs(mt, mm)
stairs(ct, cm)
legend("Matlab", "C")
title("m")