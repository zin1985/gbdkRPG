<?php
/*
 * GB/GBC Enemy Sprite Palette Editor for this GBDK RPG project.
 * Place this file at: tools/mimic_palette_editor.php
 *
 * Features:
 * - Load sprites.c and cgb_fx_runtime.c either from default project paths or uploaded files.
 * - Decode GB 2bpp enemy tile arrays.
 * - Preview S / M / L enemy sizes using CGB BG palettes.
 * - Edit palettes and per-8x8-tile palette attributes.
 * - Output C snippets for palette blocks and attr maps.
 */

$root = realpath(__DIR__ . '/..');
$defaultSpritesPath = $root . '/sprites.c';
$defaultCgbPath = $root . '/cgb_fx_runtime.c';

function h($s) { return htmlspecialchars((string)$s, ENT_QUOTES, 'UTF-8'); }

function read_uploaded_text($name) {
    if (!isset($_FILES[$name])) return null;
    if (!is_array($_FILES[$name])) return null;
    if (($_FILES[$name]['error'] ?? UPLOAD_ERR_NO_FILE) !== UPLOAD_ERR_OK) return null;
    $tmp = $_FILES[$name]['tmp_name'] ?? '';
    if (!$tmp || !is_uploaded_file($tmp)) return null;
    return file_get_contents($tmp);
}

function get_source_text($hiddenKey, $fileKey, $defaultPath, $actionValue, $loadAction) {
    $uploaded = null;
    if (($_POST['action'] ?? '') === $loadAction) $uploaded = read_uploaded_text($fileKey);
    if ($uploaded !== null) return $uploaded;
    if (isset($_POST[$hiddenKey]) && $_POST[$hiddenKey] !== '') return (string)$_POST[$hiddenKey];
    return is_file($defaultPath) ? file_get_contents($defaultPath) : '';
}

function parse_hex_array($src, $arrayName) {
    $pattern = '/const\s+unsigned\s+char\s+' . preg_quote($arrayName, '/') . '\s*\[[^\]]+\]\s*=\s*\{(.*?)\};/s';
    if (!preg_match($pattern, $src, $m)) return array();
    preg_match_all('/0x([0-9A-Fa-f]{2})/', $m[1], $mm);
    $out = array();
    foreach ($mm[1] as $hex) $out[] = hexdec($hex);
    return $out;
}

function parse_sprite_arrays($src) {
    $arrays = array();
    if ($src === '') return $arrays;
    preg_match_all('/const\s+unsigned\s+char\s+([A-Za-z_][A-Za-z0-9_]*)\s*\[([^\]]*)\]\s*=\s*\{(.*?)\};/s', $src, $mm, PREG_SET_ORDER);
    foreach ($mm as $one) {
        $name = $one[1];
        preg_match_all('/0x([0-9A-Fa-f]{2})/', $one[3], $hexes);
        $byteCount = count($hexes[1]);
        if ($byteCount <= 0 || ($byteCount % 16) !== 0) continue;
        $tileCount = intdiv($byteCount, 16);
        $guess = 'custom';
        $offsetHint = '';
        if ($tileCount == 4) $guess = 'S';
        else if ($tileCount == 16) $guess = 'M';
        else if ($tileCount == 48) {
            if ($name === 'battle_enemy_tiles') {
                $guess = 'M packed x3';
                $offsetHint = 'offset 0/16/32';
            } else {
                $guess = 'L96 or M packed';
            }
        } else if ($tileCount == 90) $guess = 'L144';
        else if ($tileCount > 16 && ($tileCount % 16) == 0) {
            $guess = 'packed M x' . intdiv($tileCount, 16);
            $offsetHint = 'offset by 16 tiles';
        }
        $arrays[] = array('name'=>$name, 'bytes'=>$byteCount, 'tiles'=>$tileCount, 'guess'=>$guess, 'hint'=>$offsetHint);
    }
    usort($arrays, function($a, $b) {
        $pa = (strpos($a['name'], 'battle_enemy') === 0) ? 0 : 1;
        $pb = (strpos($b['name'], 'battle_enemy') === 0) ? 0 : 1;
        if ($pa !== $pb) return $pa - $pb;
        return strcmp($a['name'], $b['name']);
    });
    return $arrays;
}

function parse_palettes($src) {
    $palettes = array_fill(0, 8, array_fill(0, 4, array(31,31,31)));
    if (!preg_match('/static\s+const\s+UINT16\s+cgb_normal_bg_palettes\s*\[[^\]]+\]\s*=\s*\{(.*?)\};/s', $src, $m)) return $palettes;
    preg_match_all('/CGB_RGB5\s*\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\)/', $m[1], $mm, PREG_SET_ORDER);
    $i = 0;
    foreach ($mm as $one) {
        $p = intdiv($i, 4);
        $c = $i % 4;
        if ($p < 8) $palettes[$p][$c] = array((int)$one[1], (int)$one[2], (int)$one[3]);
        $i++;
    }
    return $palettes;
}

function parse_define_values($src) {
    $defs = array(
        'BATTLE_ENEMY_ATTR_DEFAULT' => 0,
        'BATTLE_ENEMY_ATTR_BIT' => 3,
        'BATTLE_ENEMY_ATTR_BABY' => 4,
        'BATTLE_ENEMY_ATTR_MIMIC' => 2,
        'BATTLE_ENEMY_ATTR_BEAST' => 5,
        'BATTLE_ENEMY_ATTR_WARMACHINE' => 7,
        'BATTLE_ENEMY_ATTR_GOLEM_L' => 14,
    );
    preg_match_all('/#define\s+(BATTLE_ENEMY_ATTR_[A-Z0-9_]+)\s+([^\n]+)/', $src, $mm, PREG_SET_ORDER);
    foreach ($mm as $one) {
        $name = trim($one[1]);
        $expr = trim(preg_replace('/\/\*.*?\*\//', '', $one[2]));
        if (preg_match('/^(\d+)u?$/', $expr, $n)) $defs[$name] = (int)$n[1];
        else if (preg_match('/0x([0-9A-Fa-f]+)u?/', $expr, $n)) $defs[$name] = hexdec($n[1]);
    }
    return $defs;
}

function parse_mimic_attrs($src) {
    $const = parse_define_values($src);
    $attrs = array_fill(0, 16, 2);
    if (!preg_match('/static\s+void\s+battle_enemy_bg_draw_mimic_colored\s*\([^)]*\)\s*\{(.*?)\n\}/s', $src, $m)) return $attrs;
    preg_match_all('/attrs\[(\d+)\]\s*=\s*([^;]+);/', $m[1], $mm, PREG_SET_ORDER);
    foreach ($mm as $one) {
        $idx = (int)$one[1];
        $expr = trim(preg_replace('/\/\*.*?\*\//s', '', trim($one[2])));
        $value = 2;
        if (preg_match('/^(\d+)u?$/', $expr, $n)) $value = (int)$n[1];
        else if (isset($const[$expr])) $value = (int)$const[$expr];
        if ($idx >= 0 && $idx < 16) $attrs[$idx] = $value & 7;
    }
    return $attrs;
}

function gb5_to_hex($rgb) {
    $r = (int)round($rgb[0] * 255 / 31);
    $g = (int)round($rgb[1] * 255 / 31);
    $b = (int)round($rgb[2] * 255 / 31);
    return sprintf('#%02X%02X%02X', $r, $g, $b);
}

function hex_to_gb5($hex, $fallback) {
    $hex = trim($hex);
    if (preg_match('/^#?([0-9A-Fa-f]{6})$/', $hex, $m)) {
        $v = $m[1];
        $r = hexdec(substr($v,0,2));
        $g = hexdec(substr($v,2,2));
        $b = hexdec(substr($v,4,2));
        return array((int)round($r * 31 / 255), (int)round($g * 31 / 255), (int)round($b * 31 / 255));
    }
    return $fallback;
}

function size_spec($sizeMode) {
    if ($sizeMode === 'S') return array(2, 2, array(0,2,1,3));
    if ($sizeMode === 'M') return array(4, 4, array(0,2,4,6, 1,3,5,7, 8,10,12,14, 9,11,13,15));
    if ($sizeMode === 'L96') return array(12, 4, range(0, 47));
    if ($sizeMode === 'L144') return array(18, 5, range(0, 89));
    return array(4, 4, array(0,2,4,6, 1,3,5,7, 8,10,12,14, 9,11,13,15));
}

function decode_tile($bytes, $tileIndex) {
    $tile = array();
    $base = $tileIndex * 16;
    for ($y=0; $y<8; $y++) {
        $lo = $bytes[$base + $y*2] ?? 0;
        $hi = $bytes[$base + $y*2 + 1] ?? 0;
        $row = array();
        for ($x=0; $x<8; $x++) {
            $bit = 7 - $x;
            $row[] = (($lo >> $bit) & 1) | (((($hi >> $bit) & 1) << 1));
        }
        $tile[] = $row;
    }
    return $tile;
}

function build_sprite_pixels($bytes, $tilesW, $tilesH, $displayToSource, $tileOffset) {
    $w = $tilesW * 8;
    $h = $tilesH * 8;
    $pixels = array_fill(0, $h, array_fill(0, $w, 0));
    $cellCount = $tilesW * $tilesH;
    for ($cell=0; $cell<$cellCount; $cell++) {
        $srcTile = ($displayToSource[$cell] ?? $cell) + $tileOffset;
        $tile = decode_tile($bytes, $srcTile);
        $cx = ($cell % $tilesW) * 8;
        $cy = intdiv($cell, $tilesW) * 8;
        for ($y=0; $y<8; $y++) for ($x=0; $x<8; $x++) $pixels[$cy+$y][$cx+$x] = $tile[$y][$x];
    }
    return $pixels;
}

function encode_tile_from_pixels($pixels, $x0, $y0) {
    $bytes = array();
    for ($y=0; $y<8; $y++) {
        $lo = 0;
        $hi = 0;
        for ($x=0; $x<8; $x++) {
            $ci = (int)($pixels[$y0+$y][$x0+$x] ?? 0);
            if ($ci < 0) $ci = 0;
            if ($ci > 3) $ci = 3;
            $bit = 7 - $x;
            $lo |= (($ci & 1) << $bit);
            $hi |= ((($ci >> 1) & 1) << $bit);
        }
        $bytes[] = $lo;
        $bytes[] = $hi;
    }
    return $bytes;
}

function encode_sprite_pixels_to_bytes($originalBytes, $pixels, $tilesW, $tilesH, $displayToSource, $tileOffset) {
    $bytes = $originalBytes;
    $cellCount = $tilesW * $tilesH;
    for ($cell=0; $cell<$cellCount; $cell++) {
        $srcTile = ($displayToSource[$cell] ?? $cell) + $tileOffset;
        $x0 = ($cell % $tilesW) * 8;
        $y0 = intdiv($cell, $tilesW) * 8;
        $tileBytes = encode_tile_from_pixels($pixels, $x0, $y0);
        $base = $srcTile * 16;
        for ($i=0; $i<16; $i++) {
            $bytes[$base+$i] = $tileBytes[$i];
        }
    }
    return $bytes;
}

function parse_pixel_json($json, $width, $height) {
    $decoded = json_decode((string)$json, true);
    if (!is_array($decoded)) return null;
    $out = array_fill(0, $height, array_fill(0, $width, 0));
    for ($y=0; $y<$height; $y++) {
        if (!isset($decoded[$y]) || !is_array($decoded[$y])) return null;
        for ($x=0; $x<$width; $x++) {
            $v = isset($decoded[$y][$x]) ? (int)$decoded[$y][$x] : 0;
            if ($v < 0) $v = 0;
            if ($v > 3) $v = 3;
            $out[$y][$x] = $v;
        }
    }
    return $out;
}

function sprite_array_code($arrayName, $bytes) {
    $lines = array();
    for ($i=0; $i<count($bytes); $i+=16) {
        $chunk = array_slice($bytes, $i, 16);
        $vals = array();
        foreach ($chunk as $b) $vals[] = sprintf('0x%02X', $b & 0xFF);
        $lines[] = '    ' . implode(', ', $vals) . ',';
    }
    return 'const unsigned char ' . $arrayName . '[' . count($bytes) . 'u] = {' . "\n" . implode("\n", $lines) . "\n};";
}

function render_preview($pixels, $palettes, $attrs, $tilesW, $scale) {
    $height = count($pixels);
    $width = count($pixels[0] ?? array());
    $html = '<div class="sprite" style="grid-template-columns:repeat(' . $width . ',' . $scale . 'px);">';
    for ($y=0; $y<$height; $y++) {
        for ($x=0; $x<$width; $x++) {
            $cell = intdiv($y,8) * $tilesW + intdiv($x,8);
            $pal = ($attrs[$cell] ?? 0) & 7;
            $ci = $pixels[$y][$x];
            $hex = gb5_to_hex($palettes[$pal][$ci]);
            $grid = '';
            if ($x % 8 == 0) $grid .= ' border-left:1px solid rgba(255,0,0,.55);';
            if ($y % 8 == 0) $grid .= ' border-top:1px solid rgba(255,0,0,.55);';
            $html .= '<span style="width:' . $scale . 'px;height:' . $scale . 'px;background:' . $hex . ';' . $grid . '"></span>';
        }
    }
    $html .= '</div>';
    return $html;
}

function palette_block_code($palettes) {
    $lines = array();
    foreach ($palettes as $p) {
        $chunks = array();
        foreach ($p as $c) $chunks[] = sprintf('CGB_RGB5(%d,%d,%d)', $c[0], $c[1], $c[2]);
        $lines[] = '    ' . implode(', ', $chunks) . ',';
    }
    return "static const UINT16 cgb_normal_bg_palettes[8u * 4u] = {\n" . implode("\n", $lines) . "\n};";
}

function attr_code($attrs, $tilesW, $tilesH) {
    $out = array();
    for ($i=0; $i<$tilesW*$tilesH; $i++) {
        $out[] = sprintf('    attrs[%d]%s = %du; /* tile x=%d y=%d */', $i, ($i < 10 ? ' ' : ''), ($attrs[$i] ?? 0) & 7, $i % $tilesW, intdiv($i, $tilesW));
    }
    return implode("\n", $out);
}

$action = $_POST['action'] ?? '';
$spritesSrc = get_source_text('sprites_src', 'sprites_file', $defaultSpritesPath, $action, 'load_sprites');
$cgbSrc = get_source_text('cgb_src', 'cgb_file', $defaultCgbPath, $action, 'load_cgb');
$spriteArrays = parse_sprite_arrays($spritesSrc);

$sizeMode = $_POST['size_mode'] ?? $_GET['size_mode'] ?? 'M';
$arrayName = trim($_POST['array_name'] ?? $_GET['array_name'] ?? 'battle_enemy_tiles_mimic');
$tileOffset = (int)($_POST['tile_offset'] ?? $_GET['tile_offset'] ?? 0);
if ($arrayName === '') $arrayName = 'battle_enemy_tiles_mimic';
if ($tileOffset < 0) $tileOffset = 0;

list($tilesW, $tilesH, $displayToSource) = size_spec($sizeMode);
$cellCount = $tilesW * $tilesH;
$bytes = parse_hex_array($spritesSrc, $arrayName);
$palettes = parse_palettes($cgbSrc);
$attrs = ($arrayName === 'battle_enemy_tiles_mimic' && $sizeMode === 'M') ? parse_mimic_attrs($spritesSrc) : array_fill(0, $cellCount, 2);

if ($_SERVER['REQUEST_METHOD'] === 'POST' && $action !== 'load_sprites' && $action !== 'load_cgb') {
    for ($p=0; $p<8; $p++) for ($c=0; $c<4; $c++) {
        $key = "pal_{$p}_{$c}";
        if (isset($_POST[$key])) $palettes[$p][$c] = hex_to_gb5($_POST[$key], $palettes[$p][$c]);
    }
    $postedSize = $_POST['attr_count'] ?? $cellCount;
    for ($i=0; $i<$cellCount; $i++) {
        $key = "attr_{$i}";
        if (isset($_POST[$key])) $attrs[$i] = ((int)$_POST[$key]) & 7;
    }
}

if (isset($_GET['preset']) && $_GET['preset'] === 'mimic_ref') {
    $sizeMode = 'M';
    $arrayName = 'battle_enemy_tiles_mimic';
    $tileOffset = 0;
    list($tilesW, $tilesH, $displayToSource) = size_spec($sizeMode);
    $cellCount = 16;
    $attrs = array(2,2,2,2, 1,2,2,1, 1,2,2,3, 1,2,1,6);
    $palettes[1] = array(array(31,31,28), array(19,24,8), array(9,18,5), array(2,8,2));
    $palettes[2] = array(array(31,31,28), array(30,25,8), array(22,7,6), array(9,2,2));
}

$pixels = build_sprite_pixels($bytes, $tilesW, $tilesH, $displayToSource, $tileOffset);
$postedPixels = null;
if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_POST['pixel_json']) && $_POST['pixel_json'] !== '') {
    $postedPixels = parse_pixel_json($_POST['pixel_json'], $tilesW * 8, $tilesH * 8);
    if ($postedPixels !== null) $pixels = $postedPixels;
}
$editedBytes = encode_sprite_pixels_to_bytes($bytes, $pixels, $tilesW, $tilesH, $displayToSource, $tileOffset);
$scale = ($tilesW >= 18) ? 3 : (($tilesW >= 12) ? 4 : (($tilesW == 2) ? 12 : 8));
$loadedSpriteLabel = ($action === 'load_sprites' && isset($_FILES['sprites_file']['name'])) ? $_FILES['sprites_file']['name'] : basename($defaultSpritesPath);
$loadedCgbLabel = ($action === 'load_cgb' && isset($_FILES['cgb_file']['name'])) ? $_FILES['cgb_file']['name'] : basename($defaultCgbPath);
?><!doctype html>
<html lang="ja">
<head>
<meta charset="utf-8">
<title>GB/GBC Enemy Sprite Palette Editor</title>
<style>
body{font-family:system-ui,-apple-system,BlinkMacSystemFont,"Segoe UI",sans-serif;background:#f8f6ec;color:#1b1b1b;margin:24px}.wrap{display:flex;gap:24px;align-items:flex-start;flex-wrap:wrap}.panel{background:#fffdf2;border:1px solid #c9bea4;border-radius:8px;padding:16px;box-shadow:0 2px 6px rgba(0,0,0,.08);margin-bottom:16px}.sprite{display:grid;image-rendering:pixelated;border:2px solid #333;background:#fff}.sprite span{display:block;box-sizing:border-box}.pixelEditor{display:grid;image-rendering:pixelated;border:2px solid #333;background:#fff;touch-action:none}.pixelEditor button{display:block;box-sizing:border-box;border:0;border-left:1px solid rgba(255,0,0,.35);border-top:1px solid rgba(255,0,0,.35);border-radius:0;margin:0;padding:0;min-width:0;cursor:crosshair;color:#000}.pal{display:grid;grid-template-columns:70px repeat(4,110px);gap:6px;align-items:center;margin-bottom:5px}.sw{display:flex;gap:4px;align-items:center}.swatch{width:18px;height:18px;border:1px solid #333}.attrs{border-collapse:collapse}.attrs td{border:1px solid #aaa;padding:4px;text-align:center}.attrs select{width:64px}.code{width:min(900px,95vw);height:220px;font-family:ui-monospace,Consolas,monospace;font-size:12px}.small{font-size:13px;color:#555}.btn{padding:8px 12px;border:1px solid #795;background:#fff8d0;border-radius:6px;text-decoration:none;color:#000;display:inline-block}button{padding:8px 12px;border:1px solid #684;background:#e8ffd0;border-radius:6px;cursor:pointer;color:#000}input[type=file]::file-selector-button{color:#000}h1{margin-top:0}.path{font-family:ui-monospace,Consolas,monospace;font-size:12px;background:#f1ead5;padding:2px 4px;border-radius:4px}.gridform{display:grid;grid-template-columns:160px minmax(240px,1fr);gap:8px 12px;align-items:center}.hiddenSrc{display:none}.paintTools label{margin-right:12px;white-space:nowrap}.colorChip{display:inline-block;width:18px;height:18px;border:1px solid #333;vertical-align:middle;margin-right:3px}
</style>
</head>
<body>
<h1>GB/GBC 敵スプライト・パレット編集ツール</h1>
<p class="small"><strong>OBJ重ね込みが必要な敵</strong>（スケルトン、プラントアイ等）は <a class="btn" href="enemy_sprite_composite_editor.php">複合エディタ</a> を使ってください。</p>
<p class="small">スプライトファイルとCGBパレットファイルを個別に指定して読み込めます。S/M/LサイズのGB 2bppスプライトをプレビューできます。</p>
<form method="post" enctype="multipart/form-data">
<textarea class="hiddenSrc" name="sprites_src"><?php echo h($spritesSrc); ?></textarea>
<textarea class="hiddenSrc" name="cgb_src"><?php echo h($cgbSrc); ?></textarea>
<input type="hidden" name="attr_count" value="<?php echo (int)$cellCount; ?>">
<input type="hidden" id="pixelJson" name="pixel_json" value="">

<div class="panel">
  <h2>ファイル指定読み込み</h2>
  <div class="gridform">
    <label>sprites.c 指定読み込み</label>
    <div><input type="file" name="sprites_file" accept=".c,.h,.txt"> <button type="submit" name="action" value="load_sprites">sprites.cを指定して読み込み</button></div>
    <label>cgb_fx_runtime.c 指定読み込み</label>
    <div><input type="file" name="cgb_file" accept=".c,.h,.txt"> <button type="submit" name="action" value="load_cgb">CGBファイルを指定して読み込み</button></div>
  </div>
  <p class="small">未指定時は既定パスから読み込みます。既定: <span class="path"><?php echo h($defaultSpritesPath); ?></span> / <span class="path"><?php echo h($defaultCgbPath); ?></span></p>
  <p class="small">現在読み込み: スプライト=<strong><?php echo h($loadedSpriteLabel); ?></strong> / CGB=<strong><?php echo h($loadedCgbLabel); ?></strong></p>
  <p class="small">通常配置では、このツールを <span class="path">tools/mimic_palette_editor.php</span> に置くため、<strong>sprites.c は1つ上の階層</strong>、つまり <span class="path">../sprites.c</span> です。</p>
</div>

<div class="panel">
  <h2>スプライト設定</h2>
  <div class="gridform">
    <label>サイズ</label>
    <select name="size_mode">
      <option value="S" <?php echo $sizeMode==='S'?'selected':''; ?>>S 16x16 / 2x2 tiles</option>
      <option value="M" <?php echo $sizeMode==='M'?'selected':''; ?>>M 32x32 / 4x4 tiles</option>
      <option value="L96" <?php echo $sizeMode==='L96'?'selected':''; ?>>L 96x32 / 12x4 tiles</option>
      <option value="L144" <?php echo $sizeMode==='L144'?'selected':''; ?>>L 144x40 / 18x5 tiles</option>
    </select>
    <label>sprites.c内のスプライト配列</label>
    <div>
      <select name="array_name">
        <?php if (count($spriteArrays) === 0): ?>
          <option value="<?php echo h($arrayName); ?>"><?php echo h($arrayName); ?>（配列一覧を取得できません）</option>
        <?php else: ?>
          <?php foreach ($spriteArrays as $arr): ?>
            <option value="<?php echo h($arr['name']); ?>" <?php echo ($arrayName === $arr['name'] ? 'selected' : ''); ?>>
              <?php echo h($arr['name'] . ' / ' . $arr['tiles'] . ' tiles / ' . $arr['guess'] . ($arr['hint'] ? ' / ' . $arr['hint'] : '')); ?>
            </option>
          <?php endforeach; ?>
        <?php endif; ?>
      </select>
      <button type="submit" name="action" value="preview">対象スプライトを選択してプレビュー</button>
    </div>
    <label>タイルオフセット</label>
    <input type="number" name="tile_offset" value="<?php echo (int)$tileOffset; ?>" min="0">
  </div>
  <p class="small">sprites.cを指定読み込みすると、この一覧が更新されます。packed配列 <code>battle_enemy_tiles</code> を見る場合は、例として kind1=offset 16、kind2=offset 32 のように指定します。</p>
  <p><a class="btn" href="?preset=mimic_ref">ミミック参照画像寄せプリセット</a> <button type="submit" name="action" value="preview">プレビュー更新</button></p>
</div>

<div class="wrap">
  <div class="panel">
    <h2>プレビュー</h2>
    <?php echo render_preview($pixels, $palettes, $attrs, $tilesW, $scale); ?>
    <p class="small">現在: <?php echo h($arrayName); ?> / <?php echo h($sizeMode); ?> / <?php echo (int)$tilesW; ?>x<?php echo (int)$tilesH; ?> tiles。赤線は8x8タイル境界です。</p>
    <?php if (count($bytes) === 0): ?><p style="color:#b00">配列が見つかりません。sprites.c指定読み込み後、一覧から対象配列を選択してください。</p><?php endif; ?>
  </div>
  <div class="panel">
    <h2>タイル別パレット</h2>
    <table class="attrs">
    <?php for ($r=0; $r<$tilesH; $r++): ?><tr>
      <?php for ($c=0; $c<$tilesW; $c++): $i=$r*$tilesW+$c; ?><td>
        <div>#<?php echo $i; ?></div>
        <select class="attrSelect" data-cell="<?php echo $i; ?>" name="attr_<?php echo $i; ?>">
          <?php for ($p=0;$p<8;$p++): ?><option value="<?php echo $p; ?>" <?php echo ((($attrs[$i] ?? 0)&7)==$p?'selected':''); ?>>pal <?php echo $p; ?></option><?php endfor; ?>
        </select>
      </td><?php endfor; ?>
    </tr><?php endfor; ?>
    </table>
    <p class="small">プルダウンを変更すると、この画面内のプレビューは即時更新されます。Cコード出力へ反映する場合は「プレビュー更新・出力コード生成」を押してください。</p>
  </div>
  <div class="panel">
    <h2>ドット編集 4色</h2>
    <div class="paintTools">
      <?php for ($ci=0; $ci<4; $ci++): ?>
        <label><input type="radio" name="paint_color" value="<?php echo $ci; ?>" <?php echo $ci===1?'checked':''; ?>> <span id="paintChip<?php echo $ci; ?>" class="colorChip"></span>色<?php echo $ci; ?></label>
      <?php endfor; ?>
    </div>
    <p class="small">色0〜3は、各タイルに割り当てたパレット内の4色です。ドットをクリックすると選択中の色番号に変更します。</p>
    <div id="pixelEditor" class="pixelEditor" style="grid-template-columns:repeat(<?php echo (int)($tilesW*8); ?>,<?php echo (int)$scale; ?>px);"></div>
    <p><button type="submit" name="action" value="preview">ドット編集を反映してCコード生成</button></p>
  </div>
</div>

<div class="panel">
  <h2>BGパレット編集</h2>
  <?php for ($p=0; $p<8; $p++): ?>
    <div class="pal"><strong>palette <?php echo $p; ?></strong>
    <?php for ($c=0; $c<4; $c++): $hex=gb5_to_hex($palettes[$p][$c]); ?>
      <label class="sw"><span class="swatch" style="background:<?php echo $hex; ?>"></span><input type="text" name="pal_<?php echo $p.'_'.$c; ?>" value="<?php echo $hex; ?>" size="8"></label>
    <?php endfor; ?>
    </div>
  <?php endfor; ?>
  <button type="submit" name="action" value="preview">プレビュー更新・出力コード生成</button>
</div>

<div class="panel">
  <h2>出力: cgb_fx_runtime.c のパレットブロック</h2>
  <textarea id="paletteCode" class="code" readonly><?php echo h(palette_block_code($palettes)); ?></textarea>
  <p><button type="button" onclick="copyCode('paletteCode')">パレットコードをコピー</button></p>
  <h2>出力: タイル属性 attrs 部分</h2>
  <textarea id="attrCode" class="code" readonly><?php echo h(attr_code($attrs, $tilesW, $tilesH)); ?></textarea>
  <p><button type="button" onclick="copyCode('attrCode')">attrsコードをコピー</button></p>
  <h2>出力: 編集後スプライト配列</h2>
  <textarea id="spriteCode" class="code" readonly><?php echo h(sprite_array_code($arrayName, $editedBytes)); ?></textarea>
  <p><button type="button" onclick="copyCode('spriteCode')">スプライト配列コードをコピー</button></p>
</div>
</form>
<script>
const pixels = <?php echo json_encode($pixels); ?>;
const palettes = <?php echo json_encode(array_map(function($p){ return array_map('gb5_to_hex',$p); }, $palettes)); ?>;
let attrs = <?php echo json_encode(array_values(array_map(function($v){ return ((int)$v)&7; }, $attrs))); ?>;
const tilesW = <?php echo (int)$tilesW; ?>;
const widthPx = <?php echo (int)($tilesW*8); ?>;
const heightPx = <?php echo (int)($tilesH*8); ?>;
const scalePx = <?php echo (int)$scale; ?>;

function copyCode(id){const el=document.getElementById(id);el.focus();el.select();document.execCommand('copy');}
function paletteColorFor(x,y){const cell=Math.floor(y/8)*tilesW+Math.floor(x/8);const pal=attrs[cell]||0;const ci=pixels[y][x]||0;return palettes[pal][ci]||'#000000';}
function updatePixelJson(){document.getElementById('pixelJson').value=JSON.stringify(pixels);}
function selectedPaintColor(){const r=document.querySelector('input[name="paint_color"]:checked');return r?parseInt(r.value,10):1;}
function repaintEditor(){
  document.querySelectorAll('#pixelEditor button[data-x]').forEach(btn=>{const x=parseInt(btn.dataset.x,10);const y=parseInt(btn.dataset.y,10);btn.style.background=paletteColorFor(x,y);});
  for(let ci=0;ci<4;ci++){const chip=document.getElementById('paintChip'+ci); if(chip) chip.style.background=palettes[attrs[0]||0][ci]||'#000';}
  updatePixelJson();
}
function buildPixelEditor(){
  const ed=document.getElementById('pixelEditor'); if(!ed) return; ed.innerHTML='';
  for(let y=0;y<heightPx;y++){
    for(let x=0;x<widthPx;x++){
      const b=document.createElement('button'); b.type='button'; b.dataset.x=x; b.dataset.y=y;
      b.style.width=scalePx+'px'; b.style.height=scalePx+'px'; b.title='x='+x+' y='+y+' color='+pixels[y][x];
      b.addEventListener('click',()=>{pixels[y][x]=selectedPaintColor(); b.title='x='+x+' y='+y+' color='+pixels[y][x]; repaintEditor();});
      ed.appendChild(b);
    }
  }
  repaintEditor();
}
document.querySelectorAll('.attrSelect').forEach(sel=>{sel.addEventListener('change',()=>{attrs[parseInt(sel.dataset.cell,10)]=parseInt(sel.value,10)&7; repaintEditor();});});
document.querySelectorAll('input[name^="pal_"]').forEach(inp=>{inp.addEventListener('change',()=>{document.getElementById('pixelJson').value=JSON.stringify(pixels);});});
document.querySelector('form').addEventListener('submit',()=>{updatePixelJson();});
buildPixelEditor();
</script>
</body>
</html>
