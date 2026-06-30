%% PWM GATE DRIVE SIGNAL GENERATION
% Simulates the discrete switching edges of your TI C2000 ePWM Peripheral

fs = 10e6;                  % Master simulation sampling rate (10 MHz)
fsw = 100000;               % Target Switching Frequency (100 kHz)
Tsw = 1/fsw;                % Switching Period (10 microseconds)
D = 0.50;                   % Duty Cycle (50%)
t_total = 0.00005;          % Plot exactly 5 complete switching cycles

% Generate time vector and a high-frequency carrier triangle wave
t_pwm = 0:1/fs:t_total;
carrier = (sawtooth(2*pi*fsw*t_pwm, 0.5) + 1) / 2; % 0 to 1 triangle ramp

% Generate the digital gate command via comparator logic
gate_signal = carrier < D;

% Plotting the ePWM Output Waveform
figure('Name', 'TI C2000 ePWM Signal Verification', 'Color', 'w');
subplot(2,1,1);
plot(t_pwm*1e6, carrier, 'r', 'LineWidth', 1.5); hold on;
yline(D, 'b--', ['Duty Cycle Reference (D = ' num2str(D) ']'], 'LineWidth', 2);
grid on; ylabel('Normalized Carrier Ramp');
title('Internal Microcontroller PWM Comparator Logic', 'Color', 'b')


subplot(2,1,2);
plot(t_pwm*1e6, gate_signal, 'g', 'LineWidth', 2);
grid on; xlabel('Time (microseconds)'); ylabel('Gate Command (0-3.3V)');
ylim([-0.2 1.2]);

