
% First, import data from out.txt

clc

% Set upscaling factor
UpscaleFactor = 4;
OutWidth = UpscaleFactor*32-UpscaleFactor;
OutLength = UpscaleFactor*24-UpscaleFactor;

% Mesh defined - 24x32
X = 0:UpscaleFactor:OutLength;
Y = 0:UpscaleFactor:OutWidth;

[Xq,Yq] = meshgrid(0:1:OutLength - 1, 0:1:OutWidth - 1);

% Create figure
figure1 = figure('InvertHardcopy','off','Color',[1 1 1]);
colormap('hot');

% Create axes
axes1 = axes('Parent',figure1,'FontSize',8,...
    'Position',[140001 29401 336000 252000],...
    'Units','pixels');
view(axes1,[180 90]);
hold(axes1,'on');

% Create axes
axes2 = axes('Parent',figure1,'FontSize',8,...
    'Position',[616001 29401 336000 252000],...
    'Units','pixels');
view(axes2,[180 90]);
hold(axes2,'on');

% Create axes
axes3 = axes('Parent',figure1,...
    'Position',[0.026875 0.178535825545171 0.422679924242424 0.704927310488058]);
view(axes3,[90 90]);
grid(axes3,'on');
hold(axes3,'on');

% Create surf
surf(Xq,Yq,out,'Parent',axes3,'LineStyle','none');



