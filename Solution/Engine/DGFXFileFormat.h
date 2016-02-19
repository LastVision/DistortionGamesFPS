/*
-------------------------------
FILEFORMAT / FILELAYOUT
-------------------------------
*/

/*
The data is saved model / model:
First is some general data that all models need
Then the "static"-modeldata, Indices/Vertices/InputLayout/Textures, if the model has any
Then the LOD-data, if the model has any
Then the Animation-Data, if the model has any

Last but not least the Child-data, how many children the model has
Use this to recursively add childModels to the current model


-------------------------------------------------------------------------------------------------
VARIABLE	|		TYPE		|			INFO
-------------------------------------------------------------------------------------------------


--	GENERAL DATA FOR ALL MODELS
-------------------------------------------------------------------------------------------------
DGFXVersion	|		int			|	can be used to check if the model has been exported
			|					|	with a fileformat that the engine currently supports
-------------------------------------------------------------------------------------------------
isNullObject|		int			|	 is 0 if the model is NOT a nullObject
			|					|	and is 1 if the model IS a nullObject
-------------------------------------------------------------------------------------------------
isLodgroup	|		int			|	 is 0 if the model is NOT a lodGroup
			|					|	and is 1 if the model IS a lodGroup
------------------------------------------------------------------------------------------------
isAnimated	|		int			|	is 0 if the model is NOT animated
			|					|	and is 1 if the model IS animated
-------------------------------------------------------------------------------------------------


--	MODEL DATA (INDICES, VERTICES, INPUTLAYOUT, TEXTURES), ONLY IS isNullObject IS 0
-------------------------------------------------------------------------------------------------


	--	INDEX DATA
	-------------------------------------------------------------------------------------------------
	indexCount	|		int			|	how many indices there are in the model
				|					|	use to read the entire indexarray with one read
	-------------------------------------------------------------------------------------------------
	indexArray	|	unsigned int	|	all the index data in the model, can be read in one read
				|		array		|	by reading IndexCount * sizeof(int) bytes
	-------------------------------------------------------------------------------------------------


	--	VERTEX DATA
	-------------------------------------------------------------------------------------------------
	vertexCount		|		int			|	how many vertices there is in the model
					|					|	use to read entire vertexarray with one read
	-------------------------------------------------------------------------------------------------
	vertexStride	|		int			|	the size of one vertex
					|					|	use together with vertexCount when reading vertex data
	-------------------------------------------------------------------------------------------------
	vertexData		|	float array		|	all the vertexData in the model, can be read in one read
					|					|	by reading vertexCount * vertexStride * sizeof(float) bytes
	-------------------------------------------------------------------------------------------------


	--	INPUTLAYOUT DATA
	-------------------------------------------------------------------------------------------------
	layoutCount		|		int			|	the number of elements there is in the InputLayout, the
					|					|	following 3 values has to be read once for each element in the inputlayout
	-------------------------------------------------------------------------------------------------
	byteOffset		|		int			|	the byteOffset of the inputelement
					|					|	(D3D11_INPUT_ELEMENT_DESC->AlignedByteOffset)
	-------------------------------------------------------------------------------------------------
	semanticIndex	|		int			|	the semanticIndex for the inputelement
					|					|	(D3D11_INPUT_ELEMENT_DESC->SemanticIndex)
	-------------------------------------------------------------------------------------------------
	type			|		int			|	the input-element type of the element
					|					|	(ex eVertexLayout::VERTEX_POS)
	-------------------------------------------------------------------------------------------------


	--	TEXTURE DATA
	-------------------------------------------------------------------------------------------------
	textureCount	|		int			|	the number of textures in the model, the following 3
					|					|	values has to be read for each texture
	-------------------------------------------------------------------------------------------------
	textureType		|		int			|	what type of texture it is
					|					|	(ex eTextureType::ALBEDO)
	-------------------------------------------------------------------------------------------------
	lenght			|		int			|	the lenght of the texture filepath
					|					|
	-------------------------------------------------------------------------------------------------
	filepath		|	char array		|	the texture-filepath, can read entire path in one read
					|					|	by reading lenght * sizeof(char) bytes
	-------------------------------------------------------------------------------------------------

	--	ORIENTATION DATA
	-------------------------------------------------------------------------------------------------
	orientation	|	float array		|	the models base-orientation, can be read into a matrix in
				|					|	one read by reading 16 * sizeof(float) bytes
	-------------------------------------------------------------------------------------------------

--	MODEL DATA END
-------------------------------------------------------------------------------------------------


--	LOD DATA (ONLY IF isLodGroup IS 1)
-------------------------------------------------------------------------------------------------
lodCount		|		int			|	the number of lodLevels in the lodgroup
				|					|
-------------------------------------------------------------------------------------------------
lodData			|	Lod array		|	Lod is a struct containing, short myLevel and bool myUseLod
				|					|	can be read in one read by reading (sizeof(short) + sizeof(bool)) * lodCount bytes
-------------------------------------------------------------------------------------------------
threshHoldCount	|		int			|	the number of threshHolds in the lodgroup
				|					|
-------------------------------------------------------------------------------------------------
threshHoldData	|	double array	|	all of the threshHoldData, can be read in one read by
				|					|	reading sizeof(double) * threshHoldCount bytes
-------------------------------------------------------------------------------------------------



--	ANIMATION DATA
-------------------------------------------------------------------------------------------------
bindMatrix	|	float array		|	the animation's bindPoseMatrix, can be read in one read
			|					|	by rgeading sizeof(float) * 16
-------------------------------------------------------------------------------------------------

	-- HIERARCHY BONE DATA (NEED TO BE READ RECURSIVELY FOR EACH CHILD)
	-------------------------------------------------------------------------------------------------
	boneID		|		int			|	an unique ID for the bone
				|					|
	-------------------------------------------------------------------------------------------------
	nameLenght	|		int			|	lenght of the bones name
				|					|
	-------------------------------------------------------------------------------------------------
	name		|	 char array		|	the name of the bone, can be read in one read by reading
				|					|	sizeof(char) * nameLenght bytes
	-------------------------------------------------------------------------------------------------
	childCount	|		int			|	the number of child-bones this bone has
				|					|	all the above data should be read recursively for each child
	-------------------------------------------------------------------------------------------------


	-- BONE DATA
	-------------------------------------------------------------------------------------------------
	boneCount	|		int			|	the number of bones in the animation
				|					|	the following 6 values needs to read boneCount-times
	-------------------------------------------------------------------------------------------------
	nameLenght	|		int			|	lenght of the bones name
				|					|
	-------------------------------------------------------------------------------------------------
	name		|	 char array		|	the name of the bone, can be read in one read by reading
				|					|	sizeof(char) * nameLenght bytes
	-------------------------------------------------------------------------------------------------
	boneMatrix	|	float array		|	the bone's matrix, can be read in one read
				|					|	by rgeading sizeof(float) * 16
	-------------------------------------------------------------------------------------------------
	bindPose	|	float array		|	the bone's bindPoseMatrix, can be read in one read
	Matrix		|					|	by rgeading sizeof(float) * 16
	-------------------------------------------------------------------------------------------------
	frameCount	|		int			|	how many frames there is for this bone
				|					|
	-------------------------------------------------------------------------------------------------
	frameData	|	Frame array		|	Frame is a struct containing, float myTime and Matrix44f myMatrix
				|					|	can be read in one read by reading (sizeof(short) + sizeof(Matrix44f)) * frameCount bytes
	-------------------------------------------------------------------------------------------------

	-------------------------------------------------------------------------------------------------
	animation-	|		float		|	the duration of the animation
	  Lenght	|					|
	-------------------------------------------------------------------------------------------------

-- ANIMATION DATA END
-------------------------------------------------------------------------------------------------


-- CHILD DATA
-------------------------------------------------------------------------------------------------
childCount	|		int			|	the number of childModels that the model has
			|					|
-------------------------------------------------------------------------------------------------
*/

