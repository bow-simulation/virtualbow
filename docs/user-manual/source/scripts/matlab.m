% Load model file
input = loadjson('input.bow');

% Modify model data
input.string.n_strands = input.string.n_strands + 1;

% Save model file
savejson('', input, 'input.bow');

% Run a static simulation
system('virtualbow-cli static input.bow output.res');

% Load the result file
output = loadmsgpack('output.res');

% Evaluate final draw force
disp(output.statics.final_draw_force);
