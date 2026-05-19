<?php
/*
 * GB/GBC Enemy Sprite Composite Editor for this GBDK RPG project.
 * Place this file at: tools/enemy_sprite_composite_editor.php
 *
 * Features:
 * - Load sprites.c and cgb_fx_runtime.c from default paths or uploaded files.
 * - Decode / edit GB 2bpp BG enemy bodies for S/M/L sizes.
 * - Decode / edit 8x16 OBJ overlay sprites.
 * - Preview base BG, overlay strip, and composite result.
 * - Edit BG/OBJ palettes with HTML color input and a built-in color chart.
 * - Edit per-8x8 BG palette attributes and per-OBJ overlay palette/position.
 * - Output C snippets for edited sprite arrays, palette blocks, attr maps, and overlay placement arrays.
 * - Supports PLANT_EYE / PLANT_GAZE per-kind BG attr arrays and OBJ palette maps.
 * - Download all edited snippets as a single TXT handoff file.
 * - Client-side undo / redo and explicit preview repaint for dot editing.
 */

$root = realpath(__DIR__ . '/..');
$defaultSpritesPath = $root . '/sprites.c';
if (!is_file($defaultSpritesPath)) {
    foreach (array($root . '/src/sprites.c', $root . '/gbdk/_work/current_project/sprites.c', $root . '/gbdk/_work/current_project/src/sprites.c') as $p0) {
        if (is_file($p0)) { $defaultSpritesPath = $p0; break; }
    }
}
$defaultCgbPath = $root . '/cgb_fx_runtime.c';
if (!is_file($defaultCgbPath)) {
    foreach (array($root . '/src/cgb_fx_runtime.c', $root . '/gbdk/_work/current_project/cgb_fx_runtime.c', $root . '/gbdk/_work/current_project/src/cgb_fx_runtime.c') as $p0) {
        if (is_file($p0)) { $defaultCgbPath = $p0; break; }
    }
}

function h($s) { return htmlspecialchars((string)$s, ENT_QUOTES, 'UTF-8'); }

function read_uploaded_text($name) {
    if (!isset($_FILES[$name]) || !is_array($_FILES[$name])) return null;
    if (($_FILES[$name]['error'] ?? UPLOAD_ERR_NO_FILE) !== UPLOAD_ERR_OK) return null;
    $tmp = $_FILES[$name]['tmp_name'] ?? '';
    if (!$tmp || !is_uploaded_file($tmp)) return null;
    return file_get_contents($tmp);
}

function get_source_text($hiddenKey, $fileKey, $defaultPath, $actionValue, $loadAction) {
    $uploaded = null;
    if ((($_POST['action'] ?? '') === $loadAction) || (($_POST['action'] ?? '') === 'load_files')) $uploaded = read_uploaded_text($fileKey);
    if ($uploaded !== null) return $uploaded;
    if (isset($_POST[$hiddenKey]) && $_POST[$hiddenKey] !== '') return (string)$_POST[$hiddenKey];
    return is_file($defaultPath) ? file_get_contents($defaultPath) : '';
}

function parse_hex_array($src, $arrayName) {
    $pattern = '/(?:static\s+)?const\s+unsigned\s+char\s+' . preg_quote($arrayName, '/') . '\s*\[[^\]]+\]\s*=\s*\{(.*?)\};/s';
    if (!preg_match($pattern, $src, $m)) return array();
    preg_match_all('/0x([0-9A-Fa-f]{2})/', $m[1], $mm);
    $out = array();
    foreach ($mm[1] as $hex) $out[] = hexdec($hex);
    return $out;
}

function parse_sprite_arrays($src) {
    $arrays = array();
    if ($src === '') return $arrays;
    preg_match_all('/(?:static\s+)?const\s+unsigned\s+char\s+([A-Za-z_][A-Za-z0-9_]*)\s*\[([^\]]*)\]\s*=\s*\{(.*?)\};/s', $src, $mm, PREG_SET_ORDER);
    foreach ($mm as $one) {
        $name = $one[1];
        preg_match_all('/0x([0-9A-Fa-f]{2})/', $one[3], $hexes);
        $byteCount = count($hexes[1]);
        if ($byteCount <= 0 || ($byteCount % 16) !== 0) continue;
        $tileCount = intdiv($byteCount, 16);
        $guess = 'custom';
        $hint = '';
        if ($tileCount == 4) $guess = 'S 16x16';
        else if ($tileCount == 8) $guess = 'OBJ overlay x4 or 8 tiles';
        else if ($tileCount == 12) $guess = 'OBJ overlay x6';
        else if ($tileCount == 16) $guess = 'M 32x32 or OBJ overlay x8';
        else if ($tileCount == 48) { $guess = ($name === 'battle_enemy_tiles') ? 'M packed x3' : 'L96 or M packed'; $hint='offset 0/16/32'; }
        else if ($tileCount == 90) $guess = 'L144';
        else if ($tileCount > 16 && ($tileCount % 16) == 0) { $guess = 'packed M x' . intdiv($tileCount, 16); $hint='offset by 16 tiles'; }
        $arrays[] = array('name'=>$name, 'bytes'=>$byteCount, 'tiles'=>$tileCount, 'guess'=>$guess, 'hint'=>$hint);
    }
    usort($arrays, function($a, $b) {
        $pa = (strpos($a['name'], 'battle_enemy') === 0) ? 0 : ((strpos($a['name'], 'battle_') === 0) ? 1 : 2);
        $pb = (strpos($b['name'], 'battle_enemy') === 0) ? 0 : ((strpos($b['name'], 'battle_') === 0) ? 1 : 2);
        if ($pa !== $pb) return $pa - $pb;
        return strcmp($a['name'], $b['name']);
    });
    return $arrays;
}

function parse_cgb_palettes($src, $name, $fallbackAll) {
    $palettes = $fallbackAll;
    $pattern = '/static\s+const\s+UINT16\s+' . preg_quote($name, '/') . '\s*\[[^\]]+\]\s*=\s*\{(.*?)\};/s';
    if (!preg_match($pattern, $src, $m)) return $palettes;
    preg_match_all('/CGB_RGB5\s*\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\)/', $m[1], $mm, PREG_SET_ORDER);
    $i = 0;
    foreach ($mm as $one) {
        $p = intdiv($i, 4); $c = $i % 4;
        if ($p < 8) $palettes[$p][$c] = array((int)$one[1], (int)$one[2], (int)$one[3]);
        $i++;
    }
    return $palettes;
}

function parse_bg_palettes($src) {
    $fallback = array_fill(0, 8, array_fill(0, 4, array(31,31,31)));
    return parse_cgb_palettes($src, 'cgb_normal_bg_palettes', $fallback);
}

function parse_obj_palettes($src) {
    $fallback = array_fill(0, 8, array(array(31,31,31), array(23,23,20), array(11,10,9), array(0,0,0)));
    $all = parse_cgb_palettes($src, 'cgb_normal_obj_palettes', $fallback);
    if ($all === $fallback && preg_match('/static\s+const\s+UINT16\s+cgb_normal_obj_palette0\s*\[[^\]]+\]\s*=\s*\{(.*?)\};/s', $src, $m)) {
        preg_match_all('/CGB_RGB5\s*\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\)/', $m[1], $mm, PREG_SET_ORDER);
        for ($i=0; $i<count($mm) && $i<4; $i++) $all[0][$i] = array((int)$mm[$i][1], (int)$mm[$i][2], (int)$mm[$i][3]);
    }
    return $all;
}

function parse_define_values($src) {
    $defs = array('BATTLE_ENEMY_ATTR_DEFAULT'=>0, 'BATTLE_ENEMY_ATTR_BIT'=>3, 'BATTLE_ENEMY_ATTR_BABY'=>4, 'BATTLE_ENEMY_ATTR_MIMIC'=>2, 'BATTLE_ENEMY_ATTR_BEAST'=>5, 'BATTLE_ENEMY_ATTR_WARMACHINE'=>7, 'BATTLE_ENEMY_ATTR_GOLEM_L'=>14);
    preg_match_all('/#define\s+((?:BATTLE_ENEMY_ATTR|PLANT)_[A-Z0-9_]+)\s+([^\n]+)/', $src, $mm, PREG_SET_ORDER);
    foreach ($mm as $one) {
        $name = trim($one[1]);
        $expr = trim(preg_replace('/\/\*.*?\*\//', '', $one[2]));
        if (preg_match('/^(\d+)u?$/', $expr, $n)) $defs[$name] = (int)$n[1];
        else if (preg_match('/0x([0-9A-Fa-f]+)u?/', $expr, $n)) $defs[$name] = hexdec($n[1]);
    }
    return $defs;
}

function parse_uint8_array_values($src, $arrayName, $count, $default) {
    $const = parse_define_values($src);
    $vals = array_fill(0, $count, $default);
    $pattern = '/static\s+const\s+UINT8\s+' . preg_quote($arrayName, '/') . '\s*\[[^\]]+\]\s*=\s*\{(.*?)\};/s';
    if (!preg_match($pattern, $src, $m)) return $vals;
    $body = preg_replace('/\/\*.*?\*\//s', '', $m[1]);
    preg_match_all('/([A-Za-z_][A-Za-z0-9_]*|\d+)u?/', $body, $mm);
    $i = 0;
    foreach ($mm[1] as $tok) {
        if ($i >= $count) break;
        if (isset($const[$tok])) $vals[$i] = ((int)$const[$tok]) & 7;
        else if (preg_match('/^\d+$/', $tok)) $vals[$i] = ((int)$tok) & 7;
        $i++;
    }
    return $vals;
}

function suggested_attr_array_for_base($baseName) {
    $map = array(
        'battle_enemy_tiles_plant_eye' => 'battle_enemy_bg_attr_plant_eye',
        'battle_enemy_tiles_plant_gaze' => 'battle_enemy_bg_attr_plant_gaze'
    );
    return $map[$baseName] ?? '';
}


function parse_attrs_for_function($src, $funcName, $cellCount, $default) {
    $const = parse_define_values($src);
    $attrs = array_fill(0, $cellCount, $default);
    $pattern = '/static\s+void\s+' . preg_quote($funcName, '/') . '\s*\([^)]*\)\s*\{(.*?)\n\}/s';
    if (!preg_match($pattern, $src, $m)) return $attrs;
    preg_match_all('/attrs\[(\d+)\]\s*=\s*([^;]+);/', $m[1], $mm, PREG_SET_ORDER);
    foreach ($mm as $one) {
        $idx = (int)$one[1];
        $expr = trim(preg_replace('/\/\*.*?\*\//s', '', trim($one[2])));
        $value = $default;
        if (preg_match('/^(\d+)u?$/', $expr, $n)) $value = (int)$n[1];
        else if (isset($const[$expr])) $value = (int)$const[$expr];
        if ($idx >= 0 && $idx < $cellCount) $attrs[$idx] = $value & 7;
    }
    return $attrs;
}

function gb5_to_hex($rgb) {
    $r = (int)round($rgb[0] * 255 / 31); $g = (int)round($rgb[1] * 255 / 31); $b = (int)round($rgb[2] * 255 / 31);
    return sprintf('#%02X%02X%02X', $r, $g, $b);
}
function hex_to_gb5($hex, $fallback) {
    $hex = trim((string)$hex);
    if (preg_match('/^#?([0-9A-Fa-f]{6})$/', $hex, $m)) {
        $v=$m[1]; $r=hexdec(substr($v,0,2)); $g=hexdec(substr($v,2,2)); $b=hexdec(substr($v,4,2));
        return array((int)round($r*31/255), (int)round($g*31/255), (int)round($b*31/255));
    }
    return $fallback;
}

function size_spec($sizeMode) {
    if ($sizeMode === 'S') return array(2, 2, array(0,2,1,3));
    if ($sizeMode === 'M_ROW') return array(4, 4, range(0,15));
    if ($sizeMode === 'M') return array(4, 4, array(0,2,4,6, 1,3,5,7, 8,10,12,14, 9,11,13,15));
    if ($sizeMode === 'L96') return array(12, 4, range(0,47));
    if ($sizeMode === 'L144') return array(18, 5, range(0,89));
    return array(4, 4, array(0,2,4,6, 1,3,5,7, 8,10,12,14, 9,11,13,15));
}

function decode_tile($bytes, $tileIndex) {
    $tile = array(); $base = $tileIndex * 16;
    for ($y=0; $y<8; $y++) {
        $lo = $bytes[$base + $y*2] ?? 0; $hi = $bytes[$base + $y*2 + 1] ?? 0; $row = array();
        for ($x=0; $x<8; $x++) { $bit = 7 - $x; $row[] = (($lo >> $bit) & 1) | (((($hi >> $bit) & 1) << 1)); }
        $tile[] = $row;
    }
    return $tile;
}
function build_sprite_pixels($bytes, $tilesW, $tilesH, $displayToSource, $tileOffset) {
    $w=$tilesW*8; $h=$tilesH*8; $pixels=array_fill(0,$h,array_fill(0,$w,0)); $cellCount=$tilesW*$tilesH;
    for ($cell=0; $cell<$cellCount; $cell++) {
        $srcTile = ($displayToSource[$cell] ?? $cell) + $tileOffset; $tile=decode_tile($bytes,$srcTile); $cx=($cell%$tilesW)*8; $cy=intdiv($cell,$tilesW)*8;
        for ($y=0; $y<8; $y++) for ($x=0; $x<8; $x++) $pixels[$cy+$y][$cx+$x] = $tile[$y][$x];
    }
    return $pixels;
}
function encode_tile_from_pixels($pixels, $x0, $y0) {
    $bytes=array();
    for ($y=0; $y<8; $y++) { $lo=0; $hi=0; for ($x=0; $x<8; $x++) { $ci=(int)($pixels[$y0+$y][$x0+$x] ?? 0); if($ci<0)$ci=0; if($ci>3)$ci=3; $bit=7-$x; $lo|=(($ci&1)<<$bit); $hi|=((($ci>>1)&1)<<$bit); } $bytes[]=$lo; $bytes[]=$hi; }
    return $bytes;
}
function encode_sprite_pixels_to_bytes($originalBytes, $pixels, $tilesW, $tilesH, $displayToSource, $tileOffset) {
    $bytes=$originalBytes; $cellCount=$tilesW*$tilesH;
    for ($cell=0; $cell<$cellCount; $cell++) { $srcTile=($displayToSource[$cell]??$cell)+$tileOffset; $x0=($cell%$tilesW)*8; $y0=intdiv($cell,$tilesW)*8; $tileBytes=encode_tile_from_pixels($pixels,$x0,$y0); $base=$srcTile*16; for($i=0;$i<16;$i++) $bytes[$base+$i]=$tileBytes[$i]; }
    return $bytes;
}
function parse_pixel_json($json, $width, $height) {
    $decoded=json_decode((string)$json,true); if(!is_array($decoded)) return null; $out=array_fill(0,$height,array_fill(0,$width,0));
    for($y=0;$y<$height;$y++){ if(!isset($decoded[$y])||!is_array($decoded[$y])) return null; for($x=0;$x<$width;$x++){ $v=isset($decoded[$y][$x])?(int)$decoded[$y][$x]:0; if($v<0)$v=0; if($v>3)$v=3; $out[$y][$x]=$v; }}
    return $out;
}
function sprite_array_code($arrayName, $bytes, $static=true) {
    $lines=array(); for($i=0;$i<count($bytes);$i+=16){ $vals=array(); foreach(array_slice($bytes,$i,16) as $b) $vals[]=sprintf('0x%02X',$b&0xFF); $lines[]='    '.implode(', ',$vals).','; }
    return ($static?'static ':'') . 'const unsigned char ' . $arrayName . '[' . count($bytes) . 'u] = {' . "\n" . implode("\n", $lines) . "\n};";
}
function palette_block_code($name, $palettes) {
    $lines=array(); foreach($palettes as $p){ $chunks=array(); foreach($p as $c) $chunks[]=sprintf('CGB_RGB5(%d,%d,%d)',$c[0],$c[1],$c[2]); $lines[]='    '.implode(', ',$chunks).','; }
    return 'static const UINT16 '.$name.'[8u * 4u] = {' . "\n" . implode("\n", $lines) . "\n};";
}
function attr_code($attrs, $tilesW, $tilesH) {
    $out=array(); for($i=0;$i<$tilesW*$tilesH;$i++) $out[]=sprintf('    attrs[%d]%s = %du; /* tile x=%d y=%d */',$i,($i<10?' ':''),($attrs[$i]??0)&7,$i%$tilesW,intdiv($i,$tilesW)); return implode("\n",$out);
}
function overlay_position_code($ov) {
    $n=count($ov); $xs=array();$ys=array();$ps=array();$en=array(); for($i=0;$i<$n;$i++){ $xs[]=$ov[$i]['x'].'u'; $ys[]=$ov[$i]['y'].'u'; $ps[]=$ov[$i]['pal'].'u'; $en[]=($ov[$i]['en']?1:0).'u'; }
    return "#define BATTLE_ENEMY_OVERLAY_SPRITE_COUNT {$n}u\n" .
           'static const UINT8 block_x[BATTLE_ENEMY_OVERLAY_SPRITE_COUNT] = {' . implode(', ', $xs) . "};\n" .
           'static const UINT8 block_y[BATTLE_ENEMY_OVERLAY_SPRITE_COUNT] = {' . implode(', ', $ys) . "};\n" .
           'static const UINT8 block_pal[BATTLE_ENEMY_OVERLAY_SPRITE_COUNT] = {' . implode(', ', $ps) . "};\n" .
           'static const UINT8 block_enabled[BATTLE_ENEMY_OVERLAY_SPRITE_COUNT] = {' . implode(', ', $en) . "};";
}
function editor_txt_handoff($arrayName, $overlayArray, $sizeMode, $tileOffset, $overlayCount, $baseCode, $overlayCode, $bgPalCode, $objPalCode, $attrOverlayCode) {
    $lines = array();
    $lines[] = 'GB/GBC Enemy Sprite Composite Editor Export';
    $lines[] = 'Generated: ' . date('Y-m-d H:i:s');
    $lines[] = '';
    $lines[] = '[Target]';
    $lines[] = 'size_mode=' . $sizeMode;
    $lines[] = 'base_array=' . $arrayName;
    $lines[] = 'base_tile_offset=' . $tileOffset;
    $lines[] = 'overlay_array=' . $overlayArray;
    $lines[] = 'overlay_obj_count=' . $overlayCount;
    $lines[] = '';
    $lines[] = '[Base sprite C array]';
    $lines[] = $baseCode;
    $lines[] = '';
    $lines[] = '[Overlay OBJ C array]';
    $lines[] = $overlayCode;
    $lines[] = '';
    $lines[] = '[BG palettes]';
    $lines[] = $bgPalCode;
    $lines[] = '';
    $lines[] = '[OBJ palettes]';
    $lines[] = $objPalCode;
    $lines[] = '';
    $lines[] = '[Tile attrs / overlay position]';
    $lines[] = $attrOverlayCode;
    $lines[] = '';
    $lines[] = '[Notes]';
    $lines[] = '- OBJ overlay is edited as 8x16 sprites. Color 0 is transparent.';
    $lines[] = '- Current project convention reserves OAM 31-38 for enemy overlay, so 8 overlay OBJ is the safe default.';
    $lines[] = '- GB/GBC hardware limit is 40 OBJ total and 10 OBJ per scanline.';
    return implode("\n", $lines);
}

function render_preview($pixels, $palettes, $attrs, $tilesW, $scale, $transparent0=false) {
    $height=count($pixels); $width=count($pixels[0]??array()); $html='<div class="sprite" style="grid-template-columns:repeat('.$width.','.$scale.'px);">';
    for($y=0;$y<$height;$y++) for($x=0;$x<$width;$x++){ $cell=intdiv($y,8)*$tilesW+intdiv($x,8); $pal=($attrs[$cell]??0)&7; $ci=$pixels[$y][$x]; $hex=gb5_to_hex($palettes[$pal][$ci]); $bg=($transparent0&&$ci==0)?'repeating-conic-gradient(#ddd 0% 25%, #fff 0% 50%) 50% / 6px 6px':$hex; $grid=''; if($x%8==0)$grid.='border-left:1px solid rgba(255,0,0,.45);'; if($y%8==0)$grid.='border-top:1px solid rgba(255,0,0,.45);'; if($transparent0 && $x%8==0)$grid.='box-shadow:inset 2px 0 rgba(20,90,255,.55);'; if($transparent0 && $y%16==0)$grid.='border-top:2px solid rgba(20,90,255,.70);'; $html.='<span style="width:'.$scale.'px;height:'.$scale.'px;background:'.$bg.';'.$grid.'"></span>'; }
    return $html.'</div>';
}
function render_composite_preview($basePixels,$bgPals,$attrs,$tilesW,$overlayPixels,$objPals,$overlayCfg,$scale) {
    $baseW=count($basePixels[0]??array()); $baseH=count($basePixels); $minX=0;$minY=0;$maxX=$baseW;$maxY=$baseH;
    foreach($overlayCfg as $i=>$cfg){ if(!$cfg['en']) continue; $minX=min($minX,$cfg['x']); $minY=min($minY,$cfg['y']); $maxX=max($maxX,$cfg['x']+8); $maxY=max($maxY,$cfg['y']+16); }
    $outW=$maxX-$minX; $outH=$maxY-$minY; $html='<div class="sprite" style="grid-template-columns:repeat('.$outW.','.$scale.'px);">';
    for($y=$minY;$y<$maxY;$y++) for($x=$minX;$x<$maxX;$x++){
        $hex='#FFFFFF'; $has=false;
        if($x>=0&&$y>=0&&$x<$baseW&&$y<$baseH){ $cell=intdiv($y,8)*$tilesW+intdiv($x,8); $pal=($attrs[$cell]??0)&7; $ci=$basePixels[$y][$x]; $hex=gb5_to_hex($bgPals[$pal][$ci]); $has=true; }
        foreach($overlayCfg as $i=>$cfg){ if(!$cfg['en']) continue; $ox=$x-$cfg['x']; $oy=$y-$cfg['y']; if($ox>=0&&$ox<8&&$oy>=0&&$oy<16){ $sx=$i*8+$ox; $ci=$overlayPixels[$oy][$sx]??0; if($ci!=0){ $hex=gb5_to_hex($objPals[$cfg['pal']&7][$ci]); $has=true; } } }
        $grid=''; if((($x-$minX)%8)==0)$grid.='border-left:1px solid rgba(255,0,0,.45);'; if((($y-$minY)%8)==0)$grid.='border-top:1px solid rgba(255,0,0,.45);';
        $html.='<span style="width:'.$scale.'px;height:'.$scale.'px;background:'.$hex.';'.$grid.'"></span>';
    }
    return $html.'</div><p class="small">composite bounds: x='.$minX.'..'.($maxX-1).' / y='.$minY.'..'.($maxY-1).'</p>';
}
function decode_overlay_pixels($bytes,$count){ $w=$count*8; $h=16; $pixels=array_fill(0,$h,array_fill(0,$w,0)); for($i=0;$i<$count;$i++){ for($part=0;$part<2;$part++){ $tile=decode_tile($bytes,$i*2+$part); for($y=0;$y<8;$y++) for($x=0;$x<8;$x++) $pixels[$part*8+$y][$i*8+$x]=$tile[$y][$x]; }} return $pixels; }
function encode_overlay_pixels($originalBytes,$pixels,$count){ $bytes=$originalBytes; for($i=0;$i<$count;$i++){ for($part=0;$part<2;$part++){ $tb=encode_tile_from_pixels($pixels,$i*8,$part*8); $base=($i*2+$part)*16; for($j=0;$j<16;$j++) $bytes[$base+$j]=$tb[$j]; }} return $bytes; }
function sprite_array_exists($arrays, $name) {
    foreach ($arrays as $arr) if (($arr['name'] ?? '') === $name) return true;
    return false;
}
function suggested_overlay_array_for_base($baseName, $arrays) {
    $map = array(
        'battle_enemy_tiles_skeleton' => 'battle_enemy_overlay_tiles_skeleton',
        'battle_enemy_tiles_plant_eye' => 'battle_enemy_overlay_tiles_plant_eye',
        'battle_enemy_tiles_plant_gaze' => 'battle_enemy_overlay_tiles_plant_gaze'
    );
    if (isset($map[$baseName]) && sprite_array_exists($arrays, $map[$baseName])) return $map[$baseName];
    return '';
}
function suggested_attr_function_for_base($baseName) {
    $map = array(
        'battle_enemy_tiles_mimic' => 'battle_enemy_bg_draw_mimic_colored',
        'battle_enemy_tiles_skeleton' => 'battle_enemy_bg_draw_skeleton_colored',
        'battle_enemy_tiles_plant_eye' => 'battle_enemy_bg_draw_plant_eye_colored',
        'battle_enemy_tiles_plant_gaze' => 'battle_enemy_bg_draw_plant_gaze_colored'
    );
    return $map[$baseName] ?? '';
}
function overlay_count_from_bytes($bytes) {
    if (count($bytes) <= 0) return 0;
    return min(20, max(0, intdiv(count($bytes), 32))); /* 8x16 OBJ = 2 tiles = 32 bytes */
}
function default_overlay_cfg($count,$arrayName){
    $cfg=array();
    if (strpos($arrayName, 'plant_gaze') !== false) {
        $sx=array(-8,0,8,16,32,8,16,24);
        $sy=array(-7,-7,-7,-7,-7,25,25,25);
        $pal=array(2,2,2,2,2,5,5,5);
    } else if (strpos($arrayName, 'plant_eye') !== false) {
        $sx=array(-8,0,8,16,32,8,16,24);
        $sy=array(-7,-7,-7,-7,-7,25,25,25);
        $pal=array(4,4,4,4,4,3,3,3);
    } else if (strpos($arrayName, 'skeleton') !== false) {
        $sx=array(0,8,16,24,32,32,0,8);
        $sy=array(0,0,0,0,0,16,8,8);
        $pal=array(0,0,0,0,0,0,1,1);
    } else {
        $sx=array(); $sy=array(); $pal=array();
    }
    for($i=0;$i<$count;$i++){
        $cfg[$i]=array('x'=>$sx[$i]??($i*8),'y'=>$sy[$i]??0,'pal'=>$pal[$i]??0,'en'=>1);
    }
    return $cfg;
}

$action=$_POST['action']??'';
$isLoadAction = ($action === 'load_sprites' || $action === 'load_cgb' || $action === 'load_files');
$spritesSrc=get_source_text('sprites_src','sprites_file',$defaultSpritesPath,$action,'load_sprites');
$cgbSrc=get_source_text('cgb_src','cgb_file',$defaultCgbPath,$action,'load_cgb');
$spriteArrays=parse_sprite_arrays($spritesSrc);

$sizeMode=$_POST['size_mode']??$_GET['size_mode']??'M';
$arrayName=trim($_POST['array_name']??$_GET['array_name']??'battle_enemy_tiles_skeleton'); if($arrayName==='')$arrayName='battle_enemy_tiles_skeleton';
$tileOffset=(int)($_POST['tile_offset']??0); if($tileOffset<0)$tileOffset=0;
$baseContext=$sizeMode . '|' . $arrayName . '|' . $tileOffset;
$postedBaseContext=$_POST['base_context'] ?? '';
$targetChanged=($_SERVER['REQUEST_METHOD']==='POST' && $postedBaseContext!=='' && $postedBaseContext!==$baseContext);

$overlayArray=trim($_POST['overlay_array']??$_GET['overlay_array']??'');
$suggestedOverlay=suggested_overlay_array_for_base($arrayName, $spriteArrays);
if ($overlayArray==='' || $targetChanged || ($suggestedOverlay!=='' && $overlayArray===$arrayName)) {
    $overlayArray = $suggestedOverlay !== '' ? $suggestedOverlay : $overlayArray;
}
if($overlayArray==='')$overlayArray='battle_enemy_overlay_tiles_skeleton';

$overlayContextBase=$overlayArray;
$postedOverlayContext=$_POST['overlay_context'] ?? '';
$overlayTargetChanged=($_SERVER['REQUEST_METHOD']==='POST' && $postedOverlayContext!=='' && $postedOverlayContext!==$overlayContextBase);

$overlayCount=(int)($_POST['overlay_count']??0); if($overlayCount<0)$overlayCount=0; if($overlayCount>20)$overlayCount=20;

list($tilesW,$tilesH,$displayToSource)=size_spec($sizeMode); $cellCount=$tilesW*$tilesH;
$bytes=parse_hex_array($spritesSrc,$arrayName);
$overlayBytes=parse_hex_array($spritesSrc,$overlayArray);
if($overlayCount===0 || $targetChanged || $overlayTargetChanged) $overlayCount=overlay_count_from_bytes($overlayBytes);
$bgPalettes=parse_bg_palettes($cgbSrc); $objPalettes=parse_obj_palettes($cgbSrc);
$attrFunc=suggested_attr_function_for_base($arrayName);
$attrArray=suggested_attr_array_for_base($arrayName);
$defaultAttr=($arrayName==='battle_enemy_tiles_skeleton')?7:(($arrayName==='battle_enemy_tiles_mimic')?2:5);
if ($attrArray!=='' && $sizeMode==='M') $attrs=parse_uint8_array_values($spritesSrc,$attrArray,$cellCount,$defaultAttr);
else $attrs=($attrFunc!=='' && $sizeMode==='M')?parse_attrs_for_function($spritesSrc,$attrFunc,$cellCount,$defaultAttr):array_fill(0,$cellCount,$defaultAttr);
$overlayCfg=default_overlay_cfg($overlayCount,$overlayArray);
$noticeMessages=array();
if($isLoadAction) $noticeMessages[]='ファイル読み込み直後のため、前回画面のドット編集キャッシュを使わず、読み込んだC配列から再デコードしました。';
if(count($bytes)===0) $noticeMessages[]='注意: 本体配列 '.$arrayName.' のバイト列を取得できませんでした。sprites.cの配列名・読み込みファイルを確認してください。';
if($overlayArray!=='' && count($overlayBytes)===0) $noticeMessages[]='注意: 追加OBJ配列 '.$overlayArray.' のバイト列を取得できませんでした。OBJなしの場合は追加OBJ数を0にしてください。';
if($targetChanged) $noticeMessages[]='対象配列が変わったため、前回のドット編集キャッシュを破棄して再読み込みしました。';
if($suggestedOverlay!=='' && $overlayArray===$suggestedOverlay) $noticeMessages[]='本体配列に合わせて追加OBJ配列を自動選択しました: '.$overlayArray;

if($_SERVER['REQUEST_METHOD']==='POST' && !$isLoadAction){
    for($p=0;$p<8;$p++) for($c=0;$c<4;$c++){ $k="bgpal_{$p}_{$c}"; if(isset($_POST[$k])) $bgPalettes[$p][$c]=hex_to_gb5($_POST[$k],$bgPalettes[$p][$c]); $k="objpal_{$p}_{$c}"; if(isset($_POST[$k])) $objPalettes[$p][$c]=hex_to_gb5($_POST[$k],$objPalettes[$p][$c]); }
    if(!$targetChanged) {
        for($i=0;$i<$cellCount;$i++){ $k="attr_{$i}"; if(isset($_POST[$k])) $attrs[$i]=((int)$_POST[$k])&7; }
    }
    if(!$targetChanged && !$overlayTargetChanged){
        $overlayCfg=array();
        for($i=0;$i<$overlayCount;$i++){ $overlayCfg[$i]=array('x'=>(int)($_POST["ov_x_{$i}"]??($i*8)),'y'=>(int)($_POST["ov_y_{$i}"]??0),'pal'=>((int)($_POST["ov_pal_{$i}"]??0))&7,'en'=>isset($_POST["ov_en_{$i}"])); }
    }
}

$basePixels=build_sprite_pixels($bytes,$tilesW,$tilesH,$displayToSource,$tileOffset);
if(!$isLoadAction && !$targetChanged && $_SERVER['REQUEST_METHOD']==='POST' && isset($_POST['base_pixel_json']) && $_POST['base_pixel_json']!==''){ $pp=parse_pixel_json($_POST['base_pixel_json'],$tilesW*8,$tilesH*8); if($pp!==null)$basePixels=$pp; }
$overlayPixels=decode_overlay_pixels($overlayBytes,$overlayCount);
if(!$isLoadAction && !$targetChanged && !$overlayTargetChanged && $_SERVER['REQUEST_METHOD']==='POST' && isset($_POST['overlay_pixel_json']) && $_POST['overlay_pixel_json']!==''){ $pp=parse_pixel_json($_POST['overlay_pixel_json'],$overlayCount*8,16); if($pp!==null)$overlayPixels=$pp; }
$editedBaseBytes=encode_sprite_pixels_to_bytes($bytes,$basePixels,$tilesW,$tilesH,$displayToSource,$tileOffset);
$editedOverlayBytes=encode_overlay_pixels($overlayBytes,$overlayPixels,$overlayCount);
$scale=($tilesW>=18)?3:(($tilesW>=12)?4:(($tilesW==2)?12:8));
$ovScale=max(4,min(8,$scale));
$loadedSpriteLabel=(($action==='load_sprites'||$action==='load_files')&&isset($_FILES['sprites_file']['name'])&&$_FILES['sprites_file']['name']!=='')?$_FILES['sprites_file']['name']:basename($defaultSpritesPath);
$loadedCgbLabel=(($action==='load_cgb'||$action==='load_files')&&isset($_FILES['cgb_file']['name'])&&$_FILES['cgb_file']['name']!=='')?$_FILES['cgb_file']['name']:basename($defaultCgbPath);
$chartColors=array('#000000','#202020','#404040','#606060','#808080','#A0A0A0','#C0C0C0','#FFFFFF','#3A2418','#5A3A22','#8A6A36','#D8B65A','#FFF0A0','#3B0A0A','#7A1616','#C83A32','#F07A64','#0B2A12','#1F6A2C','#5DAA48','#0B1A3A','#1A4C8A','#4FA3E3','#4B2763','#8A3FA3');
$baseCode = sprite_array_code($arrayName,$editedBaseBytes,false);
$overlayCode = sprite_array_code($overlayArray,$editedOverlayBytes,true);
$bgPalCode = palette_block_code('cgb_normal_bg_palettes',$bgPalettes);
$objPalCode = palette_block_code('cgb_normal_obj_palettes',$objPalettes);
$attrOverlayCode = attr_code($attrs,$tilesW,$tilesH)."\n\n".overlay_position_code($overlayCfg);
if ($action === 'download_txt') {
    $txt = editor_txt_handoff($arrayName,$overlayArray,$sizeMode,$tileOffset,$overlayCount,$baseCode,$overlayCode,$bgPalCode,$objPalCode,$attrOverlayCode);
    header('Content-Type: text/plain; charset=UTF-8');
    header('Content-Disposition: attachment; filename=enemy_sprite_composite_export.txt');
    echo $txt;
    exit;
}
?><!doctype html><html lang="ja"><head><meta charset="utf-8"><title>GB/GBC Enemy Composite Editor</title><style>
body{font-family:system-ui,-apple-system,BlinkMacSystemFont,"Segoe UI",sans-serif;background:#f8f6ec;color:#1b1b1b;margin:24px}.wrap{display:flex;gap:24px;align-items:flex-start;flex-wrap:wrap}.panel{background:#fffdf2;border:1px solid #c9bea4;border-radius:8px;padding:16px;box-shadow:0 2px 6px rgba(0,0,0,.08);margin-bottom:16px}.sprite,.pixelEditor{display:grid;image-rendering:pixelated;border:2px solid #333;background:#fff}.sprite span,.pixelEditor button{display:block;box-sizing:border-box}.pixelEditor button{border:0;border-left:1px solid rgba(0,0,0,.10);border-top:1px solid rgba(0,0,0,.10);border-radius:0;margin:0;padding:0;min-width:0;cursor:crosshair;color:#000;position:relative}.pixelEditor button.baseTileL{border-left:2px solid rgba(255,0,0,.65)}.pixelEditor button.baseTileT{border-top:2px solid rgba(255,0,0,.65)}.pixelEditor button.objBlockL{border-left:3px solid rgba(20,90,255,.80)}.pixelEditor button.objBlockT{border-top:3px solid rgba(20,90,255,.80)}.pixelEditor button.objTileT{border-top:2px solid rgba(255,0,0,.65)}.pal{display:grid;grid-template-columns:74px repeat(4,126px);gap:6px;align-items:center;margin-bottom:5px}.sw{display:flex;gap:4px;align-items:center}.swatch,.chip{width:18px;height:18px;border:1px solid #333;display:inline-block;vertical-align:middle}.attrs,.ovtbl{border-collapse:collapse}.attrs td,.ovtbl td,.ovtbl th{border:1px solid #aaa;padding:4px;text-align:center}.attrs select{width:64px}.code{width:min(980px,95vw);height:180px;font-family:ui-monospace,Consolas,monospace;font-size:12px}.small{font-size:13px;color:#555}.btn,button{padding:8px 12px;border:1px solid #684;background:#e8ffd0;border-radius:6px;cursor:pointer;color:#000;text-decoration:none;display:inline-block}input[type=file]::file-selector-button{color:#000}.gridform{display:grid;grid-template-columns:180px minmax(240px,1fr);gap:8px 12px;align-items:center}.hiddenSrc{display:none}.tools label{margin-right:12px;white-space:nowrap}.chart{display:flex;flex-wrap:wrap;gap:4px;max-width:620px}.chart button{width:28px;height:28px;padding:0;border:1px solid #333}.activeInput{outline:3px solid #e22}.path{font-family:ui-monospace,Consolas,monospace;font-size:12px;background:#f1ead5;padding:2px 4px;border-radius:4px}
</style></head><body><h1>GB/GBC 敵スプライト複合エディタ</h1><p class="small">本体BG、追加OBJ、BG/OBJパレット、タイル属性、OBJ位置をまとめて編集できます。</p>
<form method="post" enctype="multipart/form-data"><textarea class="hiddenSrc" name="sprites_src"><?php echo h($spritesSrc); ?></textarea><textarea class="hiddenSrc" name="cgb_src"><?php echo h($cgbSrc); ?></textarea><input type="hidden" id="basePixelJson" name="base_pixel_json"><input type="hidden" id="overlayPixelJson" name="overlay_pixel_json"><input type="hidden" name="base_context" value="<?php echo h($baseContext); ?>"><input type="hidden" name="overlay_context" value="<?php echo h($overlayContextBase); ?>">
<?php if(count($noticeMessages)>0): ?><div class="panel" style="border-color:#c88;background:#fff8e8"><strong>読み込みメモ</strong><ul><?php foreach($noticeMessages as $msg): ?><li><?php echo h($msg); ?></li><?php endforeach; ?></ul></div><?php endif; ?>
<div class="panel"><h2>ファイル指定読み込み</h2><div class="gridform"><label>sprites.c</label><div><input type="file" name="sprites_file" accept=".c,.h,.txt"></div><label>cgb_fx_runtime.c</label><div><input type="file" name="cgb_file" accept=".c,.h,.txt"></div><label>一括読み込み</label><div><button name="action" value="load_files">2ファイルを一気に読み込み</button> <span class="small">片方だけ選んだ場合は、もう片方は前回内容/既定パスを使います。</span></div><label>現在</label><div><?php echo h($loadedSpriteLabel); ?> / <?php echo h($loadedCgbLabel); ?><br>既定パス: <span class="path"><?php echo h($defaultSpritesPath); ?></span></div></div></div>
<div class="panel"><h2>対象設定</h2><div class="gridform"><label>本体サイズ</label><select name="size_mode" id="sizeMode"><option value="S" <?php echo $sizeMode==='S'?'selected':''; ?>>S 16x16</option><option value="M" <?php echo $sizeMode==='M'?'selected':''; ?>>M 32x32 / エンジン描画順</option><option value="M_ROW" <?php echo $sizeMode==='M_ROW'?'selected':''; ?>>M 32x32 / row-major確認用</option><option value="L96" <?php echo $sizeMode==='L96'?'selected':''; ?>>L 96x32</option><option value="L144" <?php echo $sizeMode==='L144'?'selected':''; ?>>L 144x40</option></select><label>本体配列</label><select name="array_name" id="arrayName"><?php if(count($spriteArrays)===0): ?><option value="<?php echo h($arrayName); ?>"><?php echo h($arrayName); ?>（配列一覧なし）</option><?php else: ?><?php foreach($spriteArrays as $arr): ?><option value="<?php echo h($arr['name']); ?>" <?php echo $arrayName===$arr['name']?'selected':''; ?>><?php echo h($arr['name'].' / '.$arr['tiles'].' tiles / '.$arr['guess'].($arr['hint']?' / '.$arr['hint']:'')); ?></option><?php endforeach; ?><?php endif; ?></select><label>本体タイルオフセット</label><input type="number" name="tile_offset" id="tileOffset" min="0" value="<?php echo (int)$tileOffset; ?>"><label>追加OBJ配列</label><select name="overlay_array" id="overlayArray"><?php if(count($spriteArrays)===0): ?><option value="<?php echo h($overlayArray); ?>"><?php echo h($overlayArray); ?>（配列一覧なし）</option><?php else: ?><?php foreach($spriteArrays as $arr): ?><option value="<?php echo h($arr['name']); ?>" data-tiles="<?php echo (int)$arr['tiles']; ?>" <?php echo $overlayArray===$arr['name']?'selected':''; ?>><?php echo h($arr['name'].' / '.$arr['tiles'].' tiles'); ?></option><?php endforeach; ?><?php endif; ?></select><label>追加OBJ数</label><input type="number" name="overlay_count" id="overlayCount" min="0" max="20" value="<?php echo (int)$overlayCount; ?>"></div><p class="small">スケルトン/プラントアイなど既知の本体配列を選ぶと、対応する追加OBJ配列を自動選択します。対象変更時は古い編集キャッシュを自動破棄します。現在の安全管理枠はOAM 31〜38の8OBJです。</p><p class="small">デコード状況: 本体 <?php echo count($bytes); ?> bytes / 追加OBJ <?php echo count($overlayBytes); ?> bytes / 追加OBJ数 <?php echo (int)$overlayCount; ?></p><p><button name="action" value="preview">プレビュー更新</button></p></div>
<div class="wrap"><div class="panel"><h2>本体プレビュー</h2><?php echo render_preview($basePixels,$bgPalettes,$attrs,$tilesW,$scale,false); ?><p class="small">赤線は8x8境界。色はBGパレット属性を反映。</p></div><div class="panel"><h2>追加OBJプレビュー</h2><?php echo render_preview($overlayPixels,$objPalettes,array_map(function($v){return $v['pal'];},$overlayCfg),max(1,$overlayCount),$ovScale,true); ?><p class="small">色0は透明扱い。<strong>青い太線が8x16のOBJ管理ブロック境界</strong>、赤線が8x8タイル境界です。</p></div><div class="panel"><h2>合成プレビュー</h2><?php echo render_composite_preview($basePixels,$bgPalettes,$attrs,$tilesW,$overlayPixels,$objPalettes,$overlayCfg,$scale); ?></div></div>
<div class="wrap"><div class="panel"><h2>本体ドット編集</h2><div class="tools"><button type="button" id="undoBtn">アンドゥ</button> <button type="button" id="redoBtn">リドゥ</button> <button type="button" id="applyPreviewBtn">プレビュー反映</button><br><?php for($ci=0;$ci<4;$ci++): ?><label><input type="radio" name="paint_base" value="<?php echo $ci; ?>" <?php echo $ci===1?'checked':''; ?>> <span id="baseChip<?php echo $ci; ?>" class="chip"></span>色<?php echo $ci; ?></label><?php endfor; ?></div><div id="baseEditor" class="pixelEditor" style="grid-template-columns:repeat(<?php echo (int)($tilesW*8); ?>,<?php echo (int)$scale; ?>px)"></div><p class="small">編集欄も赤い太線で8x8タイル境界を表示します。</p></div><div class="panel"><h2>OBJドット編集</h2><div class="tools"><button type="button" id="undoBtn2">アンドゥ</button> <button type="button" id="redoBtn2">リドゥ</button> <button type="button" id="applyPreviewBtn2">プレビュー反映</button><br><?php for($ci=0;$ci<4;$ci++): ?><label><input type="radio" name="paint_ov" value="<?php echo $ci; ?>" <?php echo $ci===1?'checked':''; ?>> <span id="ovChip<?php echo $ci; ?>" class="chip"></span>色<?php echo $ci; ?></label><?php endfor; ?></div><p class="small">OBJは色0が透明です。編集欄も、青い太線=8x16実OBJ境界、赤い太線=8x8タイル境界で表示します。</p><div id="overlayEditor" class="pixelEditor" style="grid-template-columns:repeat(<?php echo (int)($overlayCount*8); ?>,<?php echo (int)$ovScale; ?>px)"></div></div></div>
<div class="panel"><h2>本体タイル別BGパレット</h2><table class="attrs"><?php for($r=0;$r<$tilesH;$r++): ?><tr><?php for($c=0;$c<$tilesW;$c++): $i=$r*$tilesW+$c; ?><td>#<?php echo $i; ?><br><select name="attr_<?php echo $i; ?>" class="attrSelect" data-cell="<?php echo $i; ?>"><?php for($p=0;$p<8;$p++): ?><option value="<?php echo $p; ?>" <?php echo ((($attrs[$i]??0)&7)===$p)?'selected':''; ?>>BG<?php echo $p; ?></option><?php endfor; ?></select></td><?php endfor; ?></tr><?php endfor; ?></table></div>
<div class="panel"><h2>追加OBJ位置・パレット</h2><table class="ovtbl"><tr><th>#</th><th>表示</th><th>x</th><th>y</th><th>OBJパレット</th></tr><?php for($i=0;$i<$overlayCount;$i++): ?><tr><td><?php echo $i; ?></td><td><input type="checkbox" name="ov_en_<?php echo $i; ?>" <?php echo ($overlayCfg[$i]['en']??1)?'checked':''; ?>></td><td><input type="number" name="ov_x_<?php echo $i; ?>" value="<?php echo h($overlayCfg[$i]['x']??0); ?>" style="width:70px"></td><td><input type="number" name="ov_y_<?php echo $i; ?>" value="<?php echo h($overlayCfg[$i]['y']??0); ?>" style="width:70px"></td><td><select name="ov_pal_<?php echo $i; ?>"><?php for($p=0;$p<8;$p++): ?><option value="<?php echo $p; ?>" <?php echo ((($overlayCfg[$i]['pal']??0)&7)===$p)?'selected':''; ?>>OBJ<?php echo $p; ?></option><?php endfor; ?></select></td></tr><?php endfor; ?></table></div>
<div class="panel"><h2>カラーパレット編集</h2><p class="small">テキスト/カラーピッカー欄をクリックしてから、下のカラーチャートを押すと色を設定できます。</p><h3>BGパレット</h3><?php for($p=0;$p<8;$p++): ?><div class="pal"><strong>BG <?php echo $p; ?></strong><?php for($c=0;$c<4;$c++): $hex=gb5_to_hex($bgPalettes[$p][$c]); ?><label class="sw"><span class="swatch" style="background:<?php echo $hex; ?>"></span><input class="colorInput" type="text" name="bgpal_<?php echo $p.'_'.$c; ?>" value="<?php echo $hex; ?>" size="8"><input class="colorInput" type="color" value="<?php echo $hex; ?>"></label><?php endfor; ?></div><?php endfor; ?><h3>OBJパレット</h3><?php for($p=0;$p<8;$p++): ?><div class="pal"><strong>OBJ <?php echo $p; ?></strong><?php for($c=0;$c<4;$c++): $hex=gb5_to_hex($objPalettes[$p][$c]); ?><label class="sw"><span class="swatch" style="background:<?php echo $hex; ?>"></span><input class="colorInput" type="text" name="objpal_<?php echo $p.'_'.$c; ?>" value="<?php echo $hex; ?>" size="8"><input class="colorInput" type="color" value="<?php echo $hex; ?>"></label><?php endfor; ?></div><?php endfor; ?><h3>カラーチャート</h3><div class="chart"><?php foreach($chartColors as $cc): ?><button type="button" class="chartColor" data-color="<?php echo $cc; ?>" style="background:<?php echo $cc; ?>" title="<?php echo $cc; ?>"></button><?php endforeach; ?></div><p><button name="action" value="preview">パレット変更を反映してプレビュー</button></p></div>
<div class="panel"><h2>出力コード</h2><p><button name="action" value="download_txt">修正結果をまとめてTXT出力</button></p><h3>本体スプライト配列</h3><textarea id="baseCode" class="code" readonly><?php echo h($baseCode); ?></textarea><p><button type="button" onclick="copyCode('baseCode')">コピー</button></p><h3>追加OBJ配列</h3><textarea id="ovCode" class="code" readonly><?php echo h($overlayCode); ?></textarea><p><button type="button" onclick="copyCode('ovCode')">コピー</button></p><h3>BGパレット</h3><textarea id="bgPalCode" class="code" readonly><?php echo h($bgPalCode); ?></textarea><p><button type="button" onclick="copyCode('bgPalCode')">コピー</button></p><h3>OBJパレット</h3><textarea id="objPalCode" class="code" readonly><?php echo h($objPalCode); ?></textarea><p><button type="button" onclick="copyCode('objPalCode')">コピー</button></p><h3>attrs / overlay位置</h3><textarea id="attrCode" class="code" readonly><?php echo h($attrOverlayCode); ?></textarea><p><button type="button" onclick="copyCode('attrCode')">コピー</button></p></div></form>
<script>
let basePixels=<?php echo json_encode($basePixels); ?>, overlayPixels=<?php echo json_encode($overlayPixels); ?>;
let bgPalettes=<?php echo json_encode(array_map(function($p){return array_map('gb5_to_hex',$p);},$bgPalettes)); ?>;
let objPalettes=<?php echo json_encode(array_map(function($p){return array_map('gb5_to_hex',$p);},$objPalettes)); ?>;
let attrs=<?php echo json_encode(array_map(function($v){return ((int)$v)&7;},$attrs)); ?>;
const overlayAutoMap={battle_enemy_tiles_skeleton:'battle_enemy_overlay_tiles_skeleton',battle_enemy_tiles_plant_eye:'battle_enemy_overlay_tiles_plant_eye',battle_enemy_tiles_plant_gaze:'battle_enemy_overlay_tiles_plant_gaze'};
let undoStack=[], redoStack=[];
function clonePixels(p){return JSON.parse(JSON.stringify(p));}
function pushHistory(){undoStack.push({base:clonePixels(basePixels), overlay:clonePixels(overlayPixels)}); if(undoStack.length>80) undoStack.shift(); redoStack=[];}
function restoreState(st){basePixels=clonePixels(st.base); overlayPixels=clonePixels(st.overlay); buildEditor('baseEditor',basePixels,baseW,baseH,baseScale,'base'); buildEditor('overlayEditor',overlayPixels,Math.max(1,ovCount)*8,16,ovScale,'ov'); repaintEditors();}
function doUndo(){if(!undoStack.length)return; redoStack.push({base:clonePixels(basePixels),overlay:clonePixels(overlayPixels)}); restoreState(undoStack.pop());}
function doRedo(){if(!redoStack.length)return; undoStack.push({base:clonePixels(basePixels),overlay:clonePixels(overlayPixels)}); restoreState(redoStack.pop());}

const tilesW=<?php echo (int)$tilesW; ?>, baseW=<?php echo (int)($tilesW*8); ?>, baseH=<?php echo (int)($tilesH*8); ?>, ovCount=<?php echo (int)$overlayCount; ?>, baseScale=<?php echo (int)$scale; ?>, ovScale=<?php echo (int)$ovScale; ?>;
function copyCode(id){const el=document.getElementById(id);el.focus();el.select();document.execCommand('copy')}
function radioVal(name){const r=document.querySelector('input[name="'+name+'"]:checked');return r?parseInt(r.value,10):1}
function colorBase(x,y){let cell=Math.floor(y/8)*tilesW+Math.floor(x/8), pal=attrs[cell]||0, ci=basePixels[y][x]||0;return bgPalettes[pal][ci]||'#000'}
function colorOv(x,y){let idx=Math.floor(x/8), sel=document.querySelector('select[name="ov_pal_'+idx+'"]'), pal=sel?parseInt(sel.value,10):0, ci=overlayPixels[y][x]||0; return ci===0?'transparent':(objPalettes[pal][ci]||'#000')}
function updateJson(){document.getElementById('basePixelJson').value=JSON.stringify(basePixels);document.getElementById('overlayPixelJson').value=JSON.stringify(overlayPixels)}
function decorateEditorCell(b,x,y,kind){
    if(kind==='base'){
        if(x%8===0)b.classList.add('baseTileL');
        if(y%8===0)b.classList.add('baseTileT');
        b.title='8x8 BG tile ('+Math.floor(x/8)+','+Math.floor(y/8)+') / px '+x+','+y;
    }else{
        if(x%8===0)b.classList.add('objBlockL');
        if(y%16===0)b.classList.add('objBlockT');
        else if(y%8===0)b.classList.add('objTileT');
        b.title='8x16 OBJ #'+Math.floor(x/8)+' / '+(y<8?'upper 8x8':'lower 8x8')+' / px '+x+','+y;
    }
}
function buildEditor(id,pix,w,h,scale,kind){const ed=document.getElementById(id);ed.innerHTML='';for(let y=0;y<h;y++)for(let x=0;x<w;x++){let b=document.createElement('button');b.type='button';b.dataset.x=x;b.dataset.y=y;b.style.width=scale+'px';b.style.height=scale+'px';decorateEditorCell(b,x,y,kind);b.onclick=()=>{pushHistory(); pix[y][x]=radioVal(kind==='base'?'paint_base':'paint_ov'); repaintEditors()};ed.appendChild(b)} }
function repaintEditors(){document.querySelectorAll('#baseEditor button').forEach(b=>{let x=+b.dataset.x,y=+b.dataset.y;b.style.background=colorBase(x,y)});document.querySelectorAll('#overlayEditor button').forEach(b=>{let x=+b.dataset.x,y=+b.dataset.y;b.style.background=colorOv(x,y)==='transparent'?'repeating-conic-gradient(#ddd 0% 25%, #fff 0% 50%) 50% / 6px 6px':colorOv(x,y)});for(let i=0;i<4;i++){let bc=document.getElementById('baseChip'+i); if(bc)bc.style.background=bgPalettes[attrs[0]||0][i]; let oc=document.getElementById('ovChip'+i); if(oc)oc.style.background=objPalettes[0][i]} updateJson()}
buildEditor('baseEditor',basePixels,baseW,baseH,baseScale,'base');buildEditor('overlayEditor',overlayPixels,Math.max(1,ovCount)*8,16,ovScale,'ov');repaintEditors();
document.querySelectorAll('.attrSelect').forEach(s=>s.onchange=()=>{attrs[+s.dataset.cell]=+s.value&7;repaintEditors()});document.querySelectorAll('select[name^="ov_pal_"]').forEach(s=>s.onchange=repaintEditors);
let activeColorInput=null;document.querySelectorAll('.colorInput').forEach(inp=>{inp.addEventListener('focus',()=>{document.querySelectorAll('.activeInput').forEach(e=>e.classList.remove('activeInput'));inp.classList.add('activeInput');activeColorInput=inp});inp.addEventListener('change',()=>{let pair=inp.parentElement.querySelectorAll('.colorInput');pair.forEach(p=>{if(p!==inp)p.value=inp.value});inp.parentElement.querySelector('.swatch').style.background=inp.value;updateJson()})});document.querySelectorAll('.chartColor').forEach(btn=>btn.onclick=()=>{if(activeColorInput){activeColorInput.value=btn.dataset.color;activeColorInput.dispatchEvent(new Event('change'))}});const arraySel=document.getElementById('arrayName'); const overlaySel=document.getElementById('overlayArray'); const overlayCountInput=document.getElementById('overlayCount');
if(arraySel){arraySel.addEventListener('change',()=>{const target=overlayAutoMap[arraySel.value]; if(target&&overlaySel){overlaySel.value=target; const opt=overlaySel.options[overlaySel.selectedIndex]; if(opt&&opt.dataset.tiles){overlayCountInput.value=Math.max(0,Math.floor(parseInt(opt.dataset.tiles,10)/2));}} document.getElementById('basePixelJson').value=''; document.getElementById('overlayPixelJson').value='';});}
if(overlaySel){overlaySel.addEventListener('change',()=>{const opt=overlaySel.options[overlaySel.selectedIndex]; if(opt&&opt.dataset.tiles){overlayCountInput.value=Math.max(0,Math.floor(parseInt(opt.dataset.tiles,10)/2));} document.getElementById('overlayPixelJson').value='';});}
['undoBtn','undoBtn2'].forEach(id=>{const b=document.getElementById(id); if(b)b.onclick=doUndo;});
['redoBtn','redoBtn2'].forEach(id=>{const b=document.getElementById(id); if(b)b.onclick=doRedo;});
['applyPreviewBtn','applyPreviewBtn2'].forEach(id=>{const b=document.getElementById(id); if(b)b.onclick=()=>{repaintEditors();updateJson();};});
document.querySelector('form').addEventListener('submit',(ev)=>{const submitter=ev.submitter||document.activeElement; if(submitter && (submitter.value==='load_sprites'||submitter.value==='load_cgb'||submitter.value==='load_files')){document.getElementById('basePixelJson').value=''; document.getElementById('overlayPixelJson').value=''; return;} updateJson();});
</script></body></html>
