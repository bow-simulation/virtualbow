pub trait StringValidation {
    // Type to be validated... basically only String
    type Value;

    // Checks if the string is a valid name for a layer or a material (only requirement: not empty)
    fn validate_name(&self) -> Result<(), Self::Value>;

    // Checks if the string represents a valid hex color code.
    // The accepted format is similar to HTML except that shortened codes are not accepted, the length must always be 7.
    fn validate_hex_color(&self) -> Result<(), Self::Value>;
}

pub trait FloatValidation {
    type Value;

    // Checks if the number is finite.
    fn validate_finite(&self) -> Result<(), Self::Value>;

    // Checks if the number is finite and strictly positive (> 0).
    fn validate_positive(&self) -> Result<(), Self::Value>;

    // Checks if the number is finite and non-negative (>= 0).
    fn validate_nonneg(&self) -> Result<(), Self::Value>;

    // Checks if the given number is finite and at least the given value (inclusive)
    fn validate_at_least(&self, min: Self::Value) -> Result<(), Self::Value>;

    // Checks if the number is finite and larger than the given value
    fn validate_larger_than(&self, min: f64) -> Result<(), f64>;

    // Checks if the number is finite and within min <= value <= max.
    fn validate_range_inclusive(&self, min: Self::Value, max: Self::Value) -> Result<(), Self::Value>;

    // Check if the number is equal to another number
    fn validate_equals(&self, value: f64) -> Result<(), f64>;
}

pub trait IntegerValidation {
    type Value;

    // Check if the number is positive
    fn validate_positive(&self) -> Result<(), Self::Value>;

    // Check if the number is greater or equal to another number
    fn validate_at_least(&self, min: Self::Value) -> Result<(), Self::Value>;
}


impl StringValidation for String {
    type Value = String;

    fn validate_name(&self) -> Result<(), String> {
        if self.is_empty() {
            return Err(self.to_string());
        }

        Ok(())
    }

    fn validate_hex_color(&self) -> Result<(), String> {
        if !self.starts_with('#') {
            return Err(self.to_string());
        }

        if self.len() != 7 {
            return Err(self.to_string());
        }

        if !self.chars().skip(1).all(|c| c.is_ascii_hexdigit()) {
            return Err(self.to_string());
        }

        Ok(())
    }
}

impl FloatValidation for f64 {
    type Value = f64;

    fn validate_finite(&self) -> Result<(), f64> {
        if !self.is_finite() {
            return Err(*self);
        }

        Ok(())
    }

    fn validate_positive(&self) -> Result<(), f64> {
        if !self.is_finite() || *self <= 0.0 {
            return Err(*self);
        }

        Ok(())
    }

    fn validate_nonneg(&self) -> Result<(), f64> {
        if !self.is_finite() || *self < 0.0 {
            return Err(*self);
        }

        Ok(())
    }

    fn validate_at_least(&self, min: f64) -> Result<(), f64> {
        if !self.is_finite() || *self < min {
            return Err(*self);
        }

        Ok(())
    }

    fn validate_larger_than(&self, min: f64) -> Result<(), f64> {
        if !self.is_finite() || *self <= min {
            return Err(*self);
        }

        Ok(())
    }

    fn validate_range_inclusive(&self, min: f64, max: f64) -> Result<(), f64> {
        if !self.is_finite() || *self < min || *self > max {
            return Err(*self);
        }

        Ok(())
    }

    fn validate_equals(&self, value: f64) -> Result<(), f64> {
        if *self != value {
            return Err(*self);
        }

        Ok(())
    }
}

impl IntegerValidation for usize {
    type Value = usize;

    fn validate_positive(&self) -> Result<(), usize> {
        if *self < 1 {
            return Err(*self);
        }

        Ok(())
    }

    fn validate_at_least(&self, min: usize) -> Result<(), usize> {
        if *self < min {
            return Err(*self);
        }

        Ok(())
    }
}


#[cfg(test)]
mod tests {
    use assert_matches::assert_matches;
    use super::{FloatValidation, IntegerValidation, StringValidation};

    #[test]
    fn test_string_validation() {
        assert_matches!("abc".to_string().validate_name(), Ok(()));
        assert_matches!("".to_string().validate_name(), Err(_));

        assert_matches!("#00ab1f".to_string().validate_hex_color(), Ok(()));
        assert_matches!("#00AB1F".to_string().validate_hex_color(), Ok(()));
        assert_matches!("".to_string().validate_hex_color(), Err(_));
        assert_matches!("abc".to_string().validate_hex_color(), Err(_));
        assert_matches!("#00ab1z".to_string().validate_hex_color(), Err(_));
        assert_matches!("#00ab1ff".to_string().validate_hex_color(), Err(_));
    }

    #[test]
    fn test_float_validation() {
        assert_matches!(1.0.validate_finite(), Ok(()));
        assert_matches!((-1.0).validate_finite(), Ok(()));
        assert_matches!(f64::NAN.validate_finite(), Err(_));
        assert_matches!(f64::INFINITY.validate_finite(), Err(_));
        assert_matches!(f64::NEG_INFINITY.validate_finite(), Err(_));

        assert_matches!(1.0.validate_positive(), Ok(()));
        assert_matches!(0.0.validate_positive(), Err(_));
        assert_matches!((-1.0).validate_positive(), Err(_));
        assert_matches!(f64::NAN.validate_positive(), Err(_));
        assert_matches!(f64::INFINITY.validate_positive(), Err(_));
        assert_matches!(f64::NEG_INFINITY.validate_positive(), Err(_));

        assert_matches!(1.0.validate_nonneg(), Ok(()));
        assert_matches!(0.0.validate_nonneg(), Ok(_));
        assert_matches!((-1.0).validate_nonneg(), Err(_));
        assert_matches!(f64::NAN.validate_nonneg(), Err(_));
        assert_matches!(f64::INFINITY.validate_nonneg(), Err(_));
        assert_matches!(f64::NEG_INFINITY.validate_nonneg(), Err(_));

        assert_matches!(5.0.validate_at_least(1.0), Ok(_));
        assert_matches!(1.0.validate_at_least(1.0), Ok(()));
        assert_matches!(0.5.validate_at_least(1.0), Err(_));
        assert_matches!(f64::NAN.validate_at_least(1.0), Err(_));
        assert_matches!(f64::INFINITY.validate_at_least(1.0), Err(_));
        assert_matches!(f64::NEG_INFINITY.validate_at_least(1.0), Err(_));

        assert_matches!(5.0.validate_larger_than(1.0), Ok(_));
        assert_matches!(1.0.validate_larger_than(1.0), Err(_));
        assert_matches!(0.5.validate_larger_than(1.0), Err(_));
        assert_matches!(f64::NAN.validate_larger_than(1.0), Err(_));
        assert_matches!(f64::INFINITY.validate_larger_than(1.0), Err(_));
        assert_matches!(f64::NEG_INFINITY.validate_larger_than(1.0), Err(_));

        assert_matches!(0.5.validate_range_inclusive(0.0, 1.0), Ok(()));
        assert_matches!(0.0.validate_range_inclusive(0.0, 1.0), Ok(()));
        assert_matches!(1.0.validate_range_inclusive(0.0, 1.0), Ok(()));
        assert_matches!((-5.0).validate_range_inclusive(0.0, 1.0), Err(_));
        assert_matches!(10.0.validate_range_inclusive(0.0, 1.0), Err(_));

        assert_matches!(1.0.validate_equals(1.0), Ok(()));
        assert_matches!(1.0.validate_equals(0.0), Err(_));
    }

    #[test]
    fn test_integer_validation() {
        assert_matches!(1.validate_positive(), Ok(_));
        assert_matches!(0.validate_positive(), Err(_));

        assert_matches!(5.validate_at_least(5), Ok(_));
        assert_matches!(9.validate_at_least(5), Ok(_));
        assert_matches!(0.validate_at_least(5), Err(_));
    }
}