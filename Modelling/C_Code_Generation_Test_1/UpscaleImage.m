function Output = UpscaleImage(InputArray)

UpscaleFactor = 2;
OutLength = 32*UpscaleFactor - UpscaleFactor;
OutWidth = 24*UpscaleFactor - UpscaleFactor;

X = 0:UpscaleFactor:OutLength;
Y = 0:UpscaleFactor:OutWidth;

[Xq,Yq] = meshgrid(0:1:OutLength - 1, 0:1:OutWidth - 1);

% Interpolation
Output = interp2(X,Y,InputArray,Xq,Yq,'cubic');

