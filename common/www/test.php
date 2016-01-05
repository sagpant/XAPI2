<?php
//获取用户提交的东西后
//试着下载

//print_r(parse_ini_file("TdxApi.License",true));

$sampleData = array(
                'License' => array(
                    'ExpireDate' => $_POST['ExpireDate'],
                    'Trial' => $_POST['Trial'],
                    'MachineID' => $_POST['MachineID'],
                ),
                'User' => array(
                    'Account' => $_POST['Account'],
                    'UserName' => $_POST['UserName'],
                ));
                
write_ini_file($sampleData, 'TdxApi.License', true);

echo exec('License.exe --License "TdxApi.License" --PrivateKey "TdxApi.PrivateKey"');
//打包两个文件
//exec();



function write_ini_file($assoc_arr, $path, $has_sections=FALSE) { 
    $content = ""; 
    if ($has_sections) { 
        foreach ($assoc_arr as $key=>$elem) { 
            $content .= "[".$key."]\n"; 
            foreach ($elem as $key2=>$elem2) { 
                if(is_array($elem2)) 
                { 
                    for($i=0;$i<count($elem2);$i++) 
                    { 
                        $content .= $key2."[] = \"".$elem2[$i]."\"\n"; 
                    } 
                } 
                else if($elem2=="") $content .= $key2." = \n"; 
                else $content .= $key2." = \"".$elem2."\"\n"; 
            } 
        } 
    } 
    else { 
        foreach ($assoc_arr as $key=>$elem) { 
            if(is_array($elem)) 
            { 
                for($i=0;$i<count($elem);$i++) 
                { 
                    $content .= $key2."[] = \"".$elem[$i]."\"\n"; 
                } 
            } 
            else if($elem=="") $content .= $key2." = \n"; 
            else $content .= $key2." = \"".$elem."\"\n"; 
        } 
    } 
    if (!$handle = fopen($path, 'w')) { 
        return false; 
    } 
    if (!fwrite($handle, $content)) { 
        return false; 
    } 
    fclose($handle); 
    return true; 
}
?>