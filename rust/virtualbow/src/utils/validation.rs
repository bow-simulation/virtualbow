// Checks if the given string represents a valid hex color code (of length 7 and in lowercase)
// https://www.geeksforgeeks.org/check-if-a-given-string-is-a-valid-hexadecimal-color-code-or-not/
pub fn is_hex_color(string: &str) -> bool {
    if !string.starts_with('#') {
        return false;
    }

    if string.len() != 7 {
        return false;
    }

    for char in string.chars().skip(1) {
        if !((char >= '0' && char <= '9') || (char >= 'a' && char <= 'f')) {
            return false;
        }
    }

    true
}

#[cfg(test)]
mod tests {
    use assert2::assert;
    use super::*;

    #[test]
    fn test_is_hex_color() {
        assert!(is_hex_color("") == false);
        assert!(is_hex_color("abc") == false);
        assert!(is_hex_color("#00ab1f") == true);
        assert!(is_hex_color("#00AB1F") == false);
        assert!(is_hex_color("#00ab1z") == false);
        assert!(is_hex_color("#00ab1ff") == false);
    }
}