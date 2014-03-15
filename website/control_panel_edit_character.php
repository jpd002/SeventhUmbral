<?php 

error_reporting(E_ALL | E_STRICT);

include("config.php");
include("database.php"); 
include("control_panel_common.php");

if(!isset($_GET["id"]))
{
	header("Location: control_panel.php");
	exit;
}

$g_characterId = $_GET["id"];

$g_tribes = array(
	0 => "Hyur Midlander Male",
	1 => "Hyur Midlander Female",
	3 => "Hyur Highlander Male",
	4 => "Elezen Wildwood Male",
	5 => "Elezen Wildwood Female",
	6 => "Elezen Duskwight Male",
	7 => "Elezen Duskwight Female",
	8 => "Lalafell Plainsfolk Male",
	9 => "Lalafell Plainsfolk Female",
	10 => "Lalafell Dunesfolk Male",
	11 => "Lalafell Dunesfolk Female",
	12 => "Miqo'te Seeker of the Sun Female",
	13 => "Miqo'te Keeper of the Moon Female",
	14 => "Roegadyn Sea Wolf Male",
	15 => "Roegadyn Hellsguard Male"
);

$g_htmlToDbFieldMapping = array(
	"characterName" => "name",
	"characterTribe" => "tribe",
	"characterSize" => "size",
	"characterVoice" => "voice",
	"characterSkinColor" => "skinColor",
	"characterHairStyle" => "hairStyle",
	"characterHairColor" => "hairColor",
	"characterHairOption" => "hairOption",
	"characterEyeColor" => "eyeColor",
	"characterFaceType" => "faceType",
	"characterFaceBrow" => "faceBrow",
	"characterFaceEye" => "faceEye",
	"characterFaceIris" => "faceIris",
	"characterFaceNose" => "faceNose",
	"characterFaceMouth" => "faceMouth",
	"characterFaceJaw" => "faceJaw",
	"characterFaceCheek" => "faceCheek",
	"characterFaceOption1" => "faceOption1",
	"characterFaceOption2" => "faceOption2",
	"characterGuardian" => "guardian",
	"characterBirthMonth" => "birthMonth",
	"characterBirthDay" => "birthDay",
	"characterHeadGear" => "headGear",
	"characterBodyGear" => "bodyGear",
	"characterLegsGear" => "legsGear",
	"characterHandsGear" => "handsGear",
	"characterFeetGear" => "feetGear"
);

//$g_dbToHtmlFieldMapping = array_flip($g_htmlToDbFieldMapping);

function SaveCharacter($databaseConnection, $htmlFieldMapping, $characterId)
{
	$characterInfo = array();
	foreach($htmlFieldMapping as $htmlFieldName => $dbFieldName)
	{
		$characterInfo[$dbFieldName] = $_POST[$htmlFieldName];
	}
	UpdateCharacterInfo($databaseConnection, $characterId, $characterInfo);
}

function GenerateTextField($characterInfo, $htmlFieldMapping, $htmlFieldName)
{
	return sprintf("<input id=\"%s\" name=\"%s\" type=\"text\" value=\"%s\" />",
		$htmlFieldName, $htmlFieldName, $characterInfo[$htmlFieldMapping[$htmlFieldName]]);
}

function GenerateSelectField($characterInfo, $htmlFieldMapping, $htmlFieldOptions, $htmlFieldName)
{
	$dbFieldName = $htmlFieldMapping[$htmlFieldName];
	$htmlText = sprintf("<select id=\"%s\" name=\"%s\">\n",
		$htmlFieldName, $htmlFieldName);
	foreach($htmlFieldOptions as $optionId => $optionName)
	{
		$htmlText .= sprintf("<option value=\"%d\" %s>%s</option>\n", 
				$optionId, 
				($optionId === (int)$characterInfo[$dbFieldName]) ? "selected" : "",
				$optionName);
	}
	$htmlText .= "</select>\n";
	return $htmlText;
}

if(isset($_POST["save"]))
{
	SaveCharacter($g_databaseConnection, $g_htmlToDbFieldMapping, $g_characterId);
	header("Location: control_panel.php");
	exit;
}

try
{
	$g_characterInfo = GetCharacterInfo($g_databaseConnection, $g_userId, $g_characterId);
}
catch(Exception $e)
{
	header("Location: control_panel.php");
	exit;
}

?>
<!DOCTYPE HTML>
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<meta charset="UTF-8">
		<title>Seventh Umbral Server</title>
		<link rel="stylesheet" type="text/css" href="css/reset.css" />
		<link rel="stylesheet" type="text/css" href="css/global.css" />
		<script type="application/ecmascript" src="FileSaver.js"></script>
		<script type="application/ecmascript">
		
			function exportCharacter()
			{
				var blob = new Blob();
				var filesaver = saveAs(blob, "video.webm");
			}
			
			function byteArrayToString(byteArray)
			{
				var i, str = '';
				for(i = 0; i < byteArray.length; i++) 
				{
					str += String.fromCharCode(byteArray[i]);
				}
				return str;
			}
			
			function decodeCharacterFile(inputArrayBuffer)
			{
				var outputArrayBuffer = new ArrayBuffer(inputArrayBuffer.byteLength);
				var inputDataView = new DataView(inputArrayBuffer);
				var outputDataView = new DataView(outputArrayBuffer);
				for(var i = 0; i < inputDataView.byteLength; i++)
				{
					outputDataView.setUint8(i, inputDataView.getUint8(i) ^ 0x73);
				}
				return outputArrayBuffer;
			}
			
			function getCharacterAttributesFromString(characterFileString)
			{
				var lineArray = characterFileString.split('\n');
				lineArray = lineArray.filter(function(str) { return str != ''; });
				var characterAttributes = [];
				for(var i = 0; i < lineArray.length; i++)
				{
					var attributeLine = lineArray[i];
					attributeItems = attributeLine.split(',');
					characterAttributes.push( 
						{ 
							"name" : attributeItems[0].trim(),
							"value" : attributeItems[3].trim()
						}
					);
				}
				return characterAttributes;
			}
			
			function getCharacterAttributeValue(attributes, attributeName)
			{
				for(var i = 0; i < attributes.length; i++)
				{
					var attribute = attributes[i];
					if(attribute.name === attributeName)
					{
						return attribute.value;
					}
				}
				return undefined;
			}
			
			function onImportFileReaderLoad(evt)
			{
				var decodedCharacterFileArray = decodeCharacterFile(evt.target.result);
				var decodedCharacterFileString = byteArrayToString(new Uint8Array(decodedCharacterFileArray));
				var characterAttributes = getCharacterAttributesFromString(decodedCharacterFileString);
				var fieldAssociations = 
				[
					[ 'characterSize',			'appearancetype_size' ],
					[ 'characterVoice', 		'appearancetype_voice' ],
					[ 'characterSkinColor',		'appearancetype_skin' ],
					[ 'characterHairStyle',		'appearancetype_hairstyle' ],
					
					[ 'characterHairColor',		'appearancetype_haircolor' ],
					[ 'characterHairOption',	'appearancetype_hairoption2' ],
					[ 'characterEyeColor', 		'appearancetype_eyecolor' ],
					[ 'characterFaceType', 		'appearancetype_facetype' ],
					
					[ 'characterFaceBrow',		'appearancetype_facebrow' ],
					[ 'characterFaceEye',		'appearancetype_faceeye' ],
					[ 'characterFaceIris',		'appearancetype_faceiris' ],
					[ 'characterFaceNose', 		'appearancetype_facenose' ],
					
					[ 'characterFaceMouth', 	'appearancetype_facemouth' ],
					[ 'characterFaceJaw', 		'appearancetype_facejaw_special' ],
					[ 'characterFaceCheek', 	'appearancetype_facecheek' ],
					[ 'characterFaceOption1', 	'appearancetype_faceoption1' ],
					
					[ 'characterFaceOption2', 	'appearancetype_faceoption2' ],
					
					[ 'characterGuardian', 		'guardian' ],
					[ 'characterBirthMonth', 	'birth_month' ],
					[ 'characterBirthDay', 		'birth_day' ],
				];
				var characterTribe = getCharacterAttributeValue(characterAttributes, "rsc_tribe");
				var characterTribeSelect = document.getElementById('characterTribe');
				for(var i = 0; i < characterTribeSelect.length; i++)
				{
					var characterTribeSelectItem = characterTribeSelect[i];
					characterTribeSelectItem.selected = characterTribeSelectItem.value === characterTribe;
				}
				for(var i = 0; i < fieldAssociations.length; i++)
				{
					var fieldAssociation = fieldAssociations[i];
					var attributeValue = getCharacterAttributeValue(characterAttributes, fieldAssociation[1]);
					document.getElementById(fieldAssociation[0]).value = attributeValue;
				}
			}
			
			function importCharacter(evt)
			{
				var file = evt.target.files[0];
				var fileReader = new FileReader();
				fileReader.readAsArrayBuffer(file);
				fileReader.onload = onImportFileReaderLoad;
			}
		
		</script>
	</head>
	<body>
		<?php include("header.php"); ?>
		<?php include("control_panel_header.php"); ?>
		<div class="edit">
			<h2>Edit Character</h2>
			<br />
			<form method="post" autocomplete="off">
				<table class="editForm">
					<tr>
						<th colspan="4">General Information</th>
					</tr>
					<tr>
						<td colspan="4">Name:</td>
					</tr>
					<tr>
						<td colspan="4"><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterName"); ?></td>
					</tr>
					<tr>
						<td colspan="4">Tribe:</td>
					</tr>
					<tr>
						<td colspan="4"><?php echo GenerateSelectField($g_characterInfo, $g_htmlToDbFieldMapping, $g_tribes, "characterTribe"); ?></td>
					</tr>
					<tr>
						<td>Guardian:</td>
						<td>Birth Month:</td>
						<td>Birth Day:</td>
						<td></td>
					</tr>
					<tr>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterGuardian"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterBirthMonth"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterBirthDay"); ?></td>
						<td></td>
					</tr>
					<tr>
						<th colspan="4">Appearance</th>
					</tr>
					<tr>
						<td>Size:</td>
						<td>Voice:</td>
						<td>Skin Color:</td>
						<td>Hair Style:</td>
					</tr>
					<tr>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterSize"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterVoice"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterSkinColor"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterHairStyle"); ?></td>
					</tr>
					<tr>
						<td>Hair Color:</td>
						<td>Hair Option:</td>
						<td>Eye Color:</td>
						<td>Face Type:</td>
					</tr>
					<tr>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterHairColor"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterHairOption"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterEyeColor"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterFaceType"); ?></td>
					</tr>
					<tr>
						<td>Face Brow:</td>
						<td>Face Eye:</td>
						<td>Face Iris:</td>
						<td>Face Nose:</td>
					</tr>
					<tr>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterFaceBrow"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterFaceEye"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterFaceIris"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterFaceNose"); ?></td>
					</tr>
					<tr>
						<td>Face Mouth:</td>
						<td>Face Jaw:</td>
						<td>Face Cheek:</td>
						<td>Face Option 1:</td>
					</tr>
					<tr>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterFaceMouth"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterFaceJaw"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterFaceCheek"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterFaceOption1"); ?></td>
					</tr>
					<tr>
						<td>Face Option 2:</td>
						<td></td>
						<td></td>
						<td></td>
					</tr>
					<tr>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterFaceOption2"); ?></td>
						<td></td>
						<td></td>
						<td></td>
					</tr>
					<tr>
						<th colspan="4">Gear</th>
					</tr>
					<tr>
						<td>Head Gear:</td>
						<td>Body Gear:</td>
						<td>Legs Gear:</td>
						<td>Hands Gear:</td>
					</tr>
					<tr>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterHeadGear"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterBodyGear"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterLegsGear"); ?></td>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterHandsGear"); ?></td>
					</tr>
					<tr>
						<td>Feet Gear:</td>
						<td></td>
						<td></td>
						<td></td>
					</tr>
					<tr>
						<td><?php echo GenerateTextField($g_characterInfo, $g_htmlToDbFieldMapping, "characterFeetGear"); ?></td>
						<td></td>
						<td></td>
						<td></td>
					</tr>
					<tr>
						<td></td>
						<td></td>
						<td colspan="2" style="text-align: right;">
							<script>
								function onImportButtonClick()
								{
									document.getElementById('importFile').click();
								}
							</script>
							<input type="file" id="importFile" style="display: none;">
							<button onclick="onImportButtonClick(); return false;">Import</button>
							<button onclick="exportCharacter(); return false;">Export</button>
							<input type="submit" name="save" value="Save" />
							<script>
								document.getElementById('importFile').addEventListener('change', importCharacter, false);
							</script>
						</td>
					</tr>
				</table>
			</form>
		</div>
		<div>
		</div>
	</body>
</html>