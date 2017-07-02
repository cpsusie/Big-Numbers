#include "StdAfx.h"

#ifdef __NEVER__

// -------------------------------------------------------------------------------------------------

HRESULT FindEncoder(
    const GUID& subtype,
    BOOL bAudio,
    IMFTransform **ppEncoder
    )
{
    HRESULT hr = S_OK;
    UINT32 count = 0;

    CLSID *ppCLSIDs = NULL;

    MFT_REGISTER_TYPE_INFO info = { 0 };

    info.guidMajorType = bAudio ? MFMediaType_Audio : MFMediaType_Video;
    info.guidSubtype = subtype;

    hr = MFTEnum(
        bAudio ? MFT_CATEGORY_AUDIO_ENCODER : MFT_CATEGORY_VIDEO_ENCODER,
        0,          // Reserved
        NULL,       // Input type
        &info,      // Output type
        NULL,       // Reserved
        &ppCLSIDs,
        &count
        );

    if (SUCCEEDED(hr) && count == 0)
    {
        hr = MF_E_TOPO_CODEC_NOT_FOUND;
    }

    // Create the first encoder in the list.

    if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(ppCLSIDs[0], NULL,
            CLSCTX_INPROC_SERVER, IID_PPV_ARGS(ppEncoder));
    }

    CoTaskMemFree(ppCLSIDs);
    return hr;
}


HRESULT CTranscoder::GetVideoOutputAvailableTypes(
    DWORD flags,
    CComPtr<IMFCollection>& pTypeCollection)
{
    HRESULT hr = S_OK;
    IMFActivate** pActivateArray = NULL;
    MFT_REGISTER_TYPE_INFO outputType;
    UINT32 nMftsFound = 0;

    do
    {
        // create the collection in which we will return the types found
        hr = MFCreateCollection(&pTypeCollection);
        BREAK_ON_FAIL(hr);

        // initialize the structure that describes the output streams that the encoders must
        // be able to produce.  In this case we want video encoders - so major type is video,
        // and we want the specified subtype
        outputType.guidMajorType = MFMediaType_Video;
        outputType.guidSubtype = MFVideoFormat_WMV3;

        // get a collection of MFTs that fit the requested pattern - video encoders,
        // with the specified subtype, and using the specified search flags
        hr = MFTEnumEx(
            MFT_CATEGORY_VIDEO_ENCODER,         // type of object to find - video encoders
            flags,                              // search flags
            NULL,                               // match all input types for an encoder
            &outputType,                        // get encoders with specified output type
            &pActivateArray,
            &nMftsFound);
        BREAK_ON_FAIL(hr);

        // now that we have an array of activation objects for matching MFTs, loop through
        // each of those MFTs, extracting all possible and available formats from each of them
        for(UINT32 x = 0; x < nMftsFound; x++)
        {
            CComPtr<IMFTransform> pEncoder;
            UINT32 typeIndex = 0;

            // activate the encoder that corresponds to the activation object
            hr = pActivateArray[x]->ActivateObject(IID_IMFTransform,
                (void**)&pEncoder);

            // while we don't have a failure, get each available output type for the MFT
            // encoder we keep looping until there are no more available types.  If there
            // are no more types for the encoder, IMFTransform::GetOutputAvailableTypes[]
            // will return MF_E_NO_MORE_TYPES
            while(SUCCEEDED(hr))
            {
                IMFMediaType* pType;

                // get the avilable type for the type index, and increment the typeIndex
                // counter
                hr = pEncoder->GetOutputAvailableType(0, typeIndex++, &pType);
                if(SUCCEEDED(hr))
                {
                    // store the type in the IMFCollection
                    hr = pTypeCollection->AddElement(pType);
                }
            }
        }
    } while(false);

    // possible valid errors that may be returned after the previous for loop is done
    if(hr == MF_E_NO_MORE_TYPES  ||  hr == MF_E_TRANSFORM_TYPE_NOT_SET)
        hr = S_OK;

    // if we successfully used MFTEnumEx() to allocate an array of the MFT activation
    // objects, then it is our responsibility to release each one and free up the memory
    // used by the array
    if(pActivateArray != NULL)
    {
        // release the individual activation objects
        for(UINT32 x = 0; x < nMftsFound; x++)
        {
            if(pActivateArray[x] != NULL)
                pActivateArray[x]->Release();
        }

        // free the memory used by the array
        CoTaskMemFree(pActivateArray);
        pActivateArray = NULL;
    }

    return hr;
}

#endif
