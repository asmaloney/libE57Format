#include "StringFunctions.h"

namespace e57
{
   template <class FTYPE> std::string floatingPointToStr( FTYPE value, int precision )
   {
      static_assert( std::is_floating_point<FTYPE>::value, "Floating point type required." );

      std::stringstream ss;
      ss << std::scientific << std::setprecision( precision ) << value;

      /// Try to remove trailing zeroes and decimal point
      /// E.g. 1.23456000000000000e+005  ==> 1.23456e+005
      /// E.g. 2.00000000000000000e+005  ==> 2e+005

      std::string s = ss.str();
      const size_t len = s.length();

      /// Split into mantissa and exponent
      /// E.g. 1.23456000000000000e+005  ==> "1.23456000000000000" + "e+005"
      std::string mantissa = s.substr( 0, len - 5 );
      std::string exponent = s.substr( len - 5, 5 );

      /// Double check that we understand the formatting
      if ( exponent[0] == 'e' )
      {
         /// Trim of any trailing zeros in mantissa
         while ( mantissa[mantissa.length() - 1] == '0' )
         {
            mantissa = mantissa.substr( 0, mantissa.length() - 1 );
         }

         /// Make one attempt to trim off trailing decimal point
         if ( mantissa[mantissa.length() - 1] == '.' )
         {
            mantissa = mantissa.substr( 0, mantissa.length() - 1 );
         }

         /// Reassemble whole floating point number
         /// Check if can drop exponent.
         if ( exponent == "e+000" )
         {
            s = mantissa;
         }
         else
         {
            s = mantissa + exponent;
         }
      }

      return s;
   }

   template std::string floatingPointToStr<float>( float value, int precision );
   template std::string floatingPointToStr<double>( double value, int precision );
}
