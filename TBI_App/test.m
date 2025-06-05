% Scan for BLE devices
devices = blelist;

% Look for your device by name
idx = find(devices.Name == "TBI_Monitor");

if isempty(idx)
    error("Device not found");
end

% Connect to the device
b = ble(devices.Address(idx(1)));
disp("Connected to: " + b.Name);

%% List characteristics
disp("Available characteristics:");
disp(b.Characteristics);

%%
commandCharObj = characteristic(b, "1840", "FFF2");
write(commandCharObj, uint8('START'), "withoutResponse");

%%
write(commandCharObj, uint8('STOP'), "withoutResponse");
%%
unsubscribe(ch);
clear device

%%
cmd = 'DISCONNECT';
write(commandCharObj, uint8(cmd), "withoutResponse");
disp("Disconnected from: " + b.Name);
clear b

%%

% Get available characteristics
c = characteristic(b, "1840", "C100");

subscribe(c, @bleCallback);

while true
    raw = read(c);   % Read latest value manually
    disp(raw);
    pause(0.1);      % Adjust polling interval as needed
end

function bleCallback()
    disp('Notification received');
end



% global dataBuffer
% dataBuffer = [];
% 
% function bleCallback(src, evt)
%     global dataBuffer
% 
%     % Get raw 24-byte data
%     raw = evt.Data;
% 
%     % Convert 8 x 3-byte signed integers
%     channels = zeros(1,8);
%     for i = 1:8
%         idx = (i-1)*3 + 1;
%         val = int32(raw(idx)) * 65536 + int32(raw(idx+1)) * 256 + int32(raw(idx+2));
% 
%         % Sign extend to 32 bits
%         if val >= 2^23
%             val = val - 2^24;
%         end
% 
%         channels(i) = double(val);
%     end
% 
%     % Append to buffer
%     dataBuffer = [dataBuffer; channels];
% 
%     % Optional live plot every N samples
%     if mod(size(dataBuffer,1), 10) == 0
%         clf;
%         plot(dataBuffer);
%         title("Simulated 8-Channel Signal");
%         ylabel("Amplitude (scaled)");
%         xlabel("Sample");
%         drawnow;
%     end
% end




