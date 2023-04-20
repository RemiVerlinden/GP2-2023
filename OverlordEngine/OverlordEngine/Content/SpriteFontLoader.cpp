#include "stdafx.h"
#include "SpriteFontLoader.h"

//SpriteFont* SpriteFontLoader::LoadContent(const ContentLoadInfo& loadInfo)
//{
//	const auto pReader = new BinaryReader();
//	pReader->Open(loadInfo.assetFullPath);
//
//	if (!pReader->Exists())
//	{
//		Logger::LogError(L"Failed to read the assetFile!\nPath: \'{}\'", loadInfo.assetSubPath);
//		return nullptr;
//	}
//
//	TODO_W7(L"Implement SpriteFontLoader >> Parse .fnt file")
//	//See BMFont Documentation for Binary Layout
//
//	//Parse the Identification bytes (B,M,F)
//	//If Identification bytes doesn't match B|M|F,
//	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
//	//return nullptr
//	//...
//
//	//Parse the version (version 3 required)
//	//If version is < 3,
//	//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
//	//return nullptr
//	//...
//
//	//Valid .fnt file >> Start Parsing!
//	//use this SpriteFontDesc to store all relevant information (used to initialize a SpriteFont object)
//	SpriteFontDesc fontDesc{};
//
//	//**********
//	// BLOCK 0 *
//	//**********
//	//Retrieve the blockId and blockSize
//	//Retrieve the FontSize [fontDesc.fontSize]
//	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
//	//Retrieve the FontName [fontDesc.fontName]
//	//...
//
//	//**********
//	// BLOCK 1 *
//	//**********
//	//Retrieve the blockId and blockSize
//	//Retrieve Texture Width & Height [fontDesc.textureWidth/textureHeight]
//	//Retrieve PageCount
//	//> if pagecount > 1
//	//	> Log Error (Only one texture per font is allowed!)
//	//Advance to Block2 (Move Reader)
//	//...
//
//	//**********
//	// BLOCK 2 *
//	//**********
//	//Retrieve the blockId and blockSize
//	//Retrieve the PageName (BinaryReader::ReadNullString)
//	//Construct the full path to the page texture file
//	//	>> page texture should be stored next to the .fnt file, pageName contains the name of the texture file
//	//	>> full texture path = asset parent_path of .fnt file (see loadInfo.assetFullPath > get parent_path) + pageName (filesystem::path::append)
//	//	>> Load the texture (ContentManager::Load<TextureData>) & Store [fontDesc.pTexture]
//
//	//**********
//	// BLOCK 3 *
//	//**********
//	//Retrieve the blockId and blockSize
//	//Retrieve Character Count (see documentation)
//	//Create loop for Character Count, and:
//	//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
//	//> Create instance of FontMetric (struct)
//	//	> Set Character (CharacterId) [FontMetric::character]
//	//	> Retrieve Xposition (store Local)
//	//	> Retrieve Yposition (store Local)
//	//	> Retrieve & Set Width [FontMetric::width]
//	//	> Retrieve & Set Height [FontMetric::height]
//	//	> Retrieve & Set OffsetX [FontMetric::offsetX]
//	//	> Retrieve & Set OffsetY [FontMetric::offsetY]
//	//	> Retrieve & Set AdvanceX [FontMetric::advanceX]
//	//	> Retrieve & Set Page [FontMetric::page]
//	//	> Retrieve Channel (BITFIELD!!!) 
//	//		> See documentation for BitField meaning [FontMetrix::channel]
//	//	> Calculate Texture Coordinates using Xposition, Yposition, fontDesc.TextureWidth & fontDesc.TextureHeight [FontMetric::texCoord]
//	//
//	//> Insert new FontMetric to the metrics [font.metrics] map
//	//	> key = (wchar_t) charId
//	//	> value = new FontMetric
//	//(loop restarts till all metrics are parsed)
//
//	//Done!
//	delete pReader;
//	return new SpriteFont(fontDesc);
//}

SpriteFont* SpriteFontLoader::LoadContent(const ContentLoadInfo& loadInfo)
{
	//************
	// OPEN FILE *
	//************
	const auto pReader = new BinaryReader();
	pReader->Open(loadInfo.assetFullPath);
	if (!pReader->Exists())
	{
		Logger::LogError(L"Failed to read the assetFile!\nPath: \'{}\'", loadInfo.assetSubPath);
		return nullptr;
	}

	//***********
	// ID BYTES *
	//***********
	std::string idBytes{};
	for (int i = 0; i < 3; ++i) { idBytes.push_back(pReader->Read<char>()); }
	//if bytes don't match BMF identification characters, exit early
	if (idBytes != "BMF")
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Not a valid .fnt font");
		return nullptr;
	}

	//****************
	// VERSION BYTES *
	//****************
	const int versionByte = pReader->Read<char>();
	//if version byte is not 3, warn user about not being compatible with other versions
	if (versionByte != 3)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Non-supported BMF version, use version 3!");
		return nullptr;
	}

	//*************
	// PARSE FONT *
	//*************
	//create empty font data structure
	SpriteFontDesc fontDescription{};

	//**********
	// BLOCK 0 *
	//**********
	//skip blockId(1 byte) and blockSize(4 bytes)
	pReader->MoveBufferPosition(5);
	//read font size
	fontDescription.fontSize = pReader->Read<short>();
	//move the binary reader to the start of the font name
	pReader->MoveBufferPosition(12);
	//read font name
	fontDescription.fontName = pReader->ReadNullString();

	//**********
	// BLOCK 1 *
	//**********
	//skip block id(1), block size(4), lineHeight(2) and base(2) values
	pReader->MoveBufferPosition(9);
	//read texture width & height
	fontDescription.textureWidth = pReader->Read<short>();
	fontDescription.textureHeight = pReader->Read<short>();
	//read page count
	const short pageCount = pReader->Read<short>();
	//if pageCount exceeds 1, warn user that we only support 1 texture per font
	if (pageCount > 1)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > page count above 1, we only support single texture fonts!");
		return nullptr;
	}
	//skip the rest of the data in this block
	pReader->MoveBufferPosition(5);


	//**********
	// BLOCK 2 *
	//**********
	//skip block id(1) and block size(4)
	pReader->MoveBufferPosition(5);
	//read page name
	const std::wstring pageName = pReader->ReadNullString();
	//calculate full path of texture file
	const std::filesystem::path texturePath = loadInfo.assetFullPath.parent_path().append(pageName);
	//load the texture
	fontDescription.pTexture = ContentManager::Load<TextureData>(texturePath);

	//**********
	// BLOCK 3 *
	//**********
	//skip block id(1)
	pReader->MoveBufferPosition(1);
	//get block size and calculate character count (each character is described in 20 bytes)
	const int blockSize = pReader->Read<int>();
	const int characterCount = blockSize / 20;
	//loop over every character
	for (int i = 0; i < characterCount; ++i)
	{
		//create empty font metric data struct
		FontMetric characterData{};

		//read character id
		const wchar_t characterId = static_cast<wchar_t>(pReader->Read<unsigned int>());
		characterData.character = characterId;
		//read x & y position
		unsigned short xPosition = pReader->Read<unsigned short>();
		unsigned short yPosition = pReader->Read<unsigned short>();
		//read width & height
		characterData.width = pReader->Read<unsigned short>();
		characterData.height = pReader->Read<unsigned short>();
		//read x & y offsets
		characterData.offsetX = pReader->Read<short>();
		characterData.offsetY = pReader->Read<short>();
		//read x advance
		characterData.advanceX = pReader->Read<short>();
		//read page number
		characterData.page = pReader->Read<unsigned char>();
		//read channel
		characterData.channel = pReader->Read<char>();
		//convert to our data format (bitflag value => index of rgba vector)
		std::unordered_map<char, char> conversionMap{ {1,2},{2,1},{4,0},{8,3} };
		characterData.channel = conversionMap.at(characterData.channel);
		//calculate texture coordinates in [0,1] range
		characterData.texCoord = XMFLOAT2(float(xPosition) / float(fontDescription.textureWidth), float(yPosition) / float(fontDescription.textureHeight));

		//add character info to character info map
		fontDescription.metrics.insert({ characterId, characterData });
	}


	//Done!
	delete pReader;
	return new SpriteFont(fontDescription);
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
