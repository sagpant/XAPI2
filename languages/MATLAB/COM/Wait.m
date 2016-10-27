% 需要循环等待才行
function Wait()

global wait_lock;
cnt = 10;
while isempty(wait_lock) || wait_lock==0
    cnt = cnt - 1;
    if cnt<0
        break;
    end
    
    pause(1);
end

end