# Curve: Line of length l in the x direction
function line(l)
    return function eval(n)
        return [n*l, 0, 0]
    end
end

# Curve: Arc of length l and radius r, starting in the x direction
function arc(l, r)
    return function eval(n)
        φ = n*l/r
        return [r*sin(φ), r*(1 - cos(φ)), φ]
    end
end

# Curve: Series of k alternating semicircles with a total length of l
function waves(l, k)
    r = l/(pi*k)

    return function eval(n)
        i = floor(k*n)     # Index of the current arc (0-based)
        c = r*(1 + 2*i)    # Center of the current arc
        α = (n*k - i)*pi   # Angle from arc center
        return [c - r*cos(α), (-1)^(i)*r*sin(α), (-1)^(i)*(pi/2 - α)]
    end
end