function Output = UpscaleImage(InputArray)

UpscaleFactor = 4;
OutLength = 32*UpscaleFactor - UpscaleFactor;
OutWidth = 24*UpscaleFactor - UpscaleFactor;

% Mesh defined - 24x32
[X,Y] = meshgrid(0:UpscaleFactor:OutLength,...
    0:UpscaleFactor:OutWidth);
% Mesh define
[Xq,Yq] = meshgrid(0:1:OutLength-1, 0:1:OutWidth-1);
% Interpolation
Output = interp2(X,Y,InputArray,Xq,Yq,'cubic');

