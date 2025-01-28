// Checks if the given string represents a valid hex color code.
// Similar to HTML but does not accept shortened codes, length must always be 7.
pub fn is_hex_color(string: &str) -> bool {
    if !string.starts_with('#') {
        return false;
    }

    if string.len() != 7 {
        return false;
    }

    for char in string.chars().skip(1) {
        if !char.is_ascii_hexdigit() {
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
        assert!(is_hex_color("#00AB1F") == true);
        assert!(is_hex_color("#00ab1z") == false);
        assert!(is_hex_color("#00ab1ff") == false);
    }
}