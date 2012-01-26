#include "itkImage.h"
#include "itkVectorImage.h"
#include "itkLabelMap.h"
#include "itkLabelObject.h"
#include "itkNumericTraits.h"
#include "itkNumericTraitsVariableLengthVectorPixel.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkComposeImageFilter.h"

#include "sitkJoinSeriesImageFilter.h"
#include "itkJoinSeriesImageFilter.h"



namespace itk {
namespace simple {

//-----------------------------------------------------------------------------

//
// Default constructor that initializes parameters
//
JoinSeriesImageFilter::JoinSeriesImageFilter ()
{

  this->m_Spacing = 1.0;
  this->m_Origin = 0.0;

  this->m_MemberFactory.reset( new detail::MemberFunctionFactory<MemberFunctionType>( this ) );

  this->m_MemberFactory->RegisterMemberFunctions< PixelIDTypeList, 2 > ();

}

//
// ToString
//
std::string JoinSeriesImageFilter::ToString() const
{
  std::ostringstream out;
  out << "itk::simple::JoinSeriesImageFilter\n";
  out << "\tSpacing: " << this->m_Spacing << std::endl;
  out << "\tOrigin: " << this->m_Origin << std::endl;


  return out.str();
}

Image JoinSeriesImageFilter::Execute ( const Image &image )
{
  Image constImage = image;
  std::vector<Image> images;
  images.push_back(constImage);
  return this->Execute( images );
}

//
// Execute
//
Image JoinSeriesImageFilter::Execute ( const std::vector<Image> &images )
{
  if ( images.empty() )
    {
    sitkExceptionMacro( "Atleast one input is required" );
    }

  PixelIDValueType type = images.front().GetPixelIDValue();
  unsigned int dimension = images.front().GetDimension();

  return this->m_MemberFactory->GetMemberFunction( type, dimension )( images );
}

//-----------------------------------------------------------------------------

//
// ExecuteInternal
//
template <class TImageType>
Image JoinSeriesImageFilter::ExecuteInternal ( const std::vector<Image> & images  )
{
  // Define the input and output image types
  typedef TImageType     InputImageType;

  typedef itk::Image<typename InputImageType::PixelType, 3> OutputImageType;

  typedef itk::JoinSeriesImageFilter<InputImageType,  OutputImageType> FilterType;
  // Set up the ITK filter
  typename FilterType::Pointer filter = FilterType::New();

  for ( unsigned int i = 0; i < images.size(); ++i )
    {
    // Get the pointer to the ITK image contained in image1
    typename InputImageType::ConstPointer image =
      dynamic_cast <const InputImageType*> ( images[i].GetITKBase() );

        // Check that ITK image pointer was properly retrieved
        if ( image.IsNull() )
          {
          sitkExceptionMacro( "The " << i << " input does not match the dimension or type of the first!" );
          }

          filter->SetInput( i, image );
        }


  filter->SetSpacing( this->m_Spacing );
  filter->SetOrigin( this->m_Origin );

  // Run the ITK filter and return the output as a SimpleITK image
  filter->Update();


  return Image( filter->GetOutput() );
}

//-----------------------------------------------------------------------------


//
// Function to run the Execute method of this filter
//
Image JoinSeries ( const std::vector<Image>& images, double inSpacing, double inOrigin )
{
  JoinSeriesImageFilter filter;
  return filter.SetSpacing( inSpacing ).SetOrigin( inOrigin ).Execute ( images );
}

} // end namespace simple
} // end namespace itk
