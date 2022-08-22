% Load model file
input = loadjson('input.bow');

% Modify model data
input.string.n_strands = input.string.n_strands + 1;

% Save model file
savejson('', input, 'input.bow');

% Run a static simulation
system('virtualbow-slv --static input.bow output.res');

% Load the result file
output = loadmsgpack('output.res');

% Evaluate stresses
He_back = output.setup.limb_properties.layers{1}.He_back;
Hk_back = output.setup.limb_properties.layers{1}.Hk_back;

epsilon = output.statics.states.epsilon(:,end);
kappa   = output.statics.states.kappa(:,end);
sigma = He_back*epsilon + Hk_back*kappa;

disp(max(sigma));
