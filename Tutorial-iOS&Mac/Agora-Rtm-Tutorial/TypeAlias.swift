//
//  TypeAlias.swift
//  AgoraPremium
//
//  Created by GongYuhua on 4/11/16.
//  Copyright © 2016 Agora. All rights reserved.
//

#if os(iOS)
    import UIKit
#else
    import Cocoa
#endif

//MARK: - Color
#if os(iOS)
typealias AGColor = UIColor
#else
typealias AGColor = NSColor
#endif
extension AGColor {
    convenience init(hex: Int, alpha: CGFloat = 1) {
        func transform(_ input: Int, offset: Int = 0) -> CGFloat {
            let value = (input >> offset) & 0xff
            return CGFloat(value) / 255
        }
        
        self.init(red: transform(hex, offset: 16),
                  green: transform(hex, offset: 8),
                  blue: transform(hex),
                  alpha: alpha)
    }
    
    func rgbValue() -> (red: CGFloat, green: CGFloat, blue: CGFloat) {
        var red: CGFloat = 0
        var green: CGFloat = 0
        var blue: CGFloat = 0
        
        getRed(&red, green: &green, blue: &blue, alpha: nil)
        
        return (red * 255, green * 255, blue * 255)
    }
}

//MARK: - Font
#if os(iOS)
typealias AGFont = UIFont
#else
typealias AGFont = NSFont
#endif

//MARK: - Image
#if os(iOS)
public typealias AGImage = UIImage
#else
public typealias AGImage = NSImage
#endif

extension AGImage {
    func compressQualityWith(maxLength: Int) -> Data? {
        let compression: Float = 1
        
        #if os(iOS)
        guard var data = self.jpegData(compressionQuality: CGFloat(compression)) else {
            return nil
        }
        #else
        guard var data = self.tiffRepresentation else {
            return nil
        }
        
        guard let _ = NSBitmapImageRep.init(data: data) else {
            return nil;
        }
        #endif
        
        if data.count < maxLength {
            return data
        }
        let max = Double(maxLength)
        let devision = Double(data.count)
        let comp = max / devision
        
        #if os(iOS)
        guard let compressData = self.jpegData(compressionQuality: CGFloat(comp)) else {
            return nil
        }
        #else
        guard let compressData = compressedJPEG(with: Double(comp)) else {
            return nil
        }
        #endif
        
        return compressData
    }
    
    #if os(macOS)
    func compressedJPEG(with factor: Double) -> Data? {
        guard let tiff = tiffRepresentation else { return nil }
        guard let imageRep = NSBitmapImageRep(data: tiff) else { return nil }
        
        let options: [NSBitmapImageRep.PropertyKey: Any] = [
            .compressionFactor: factor
        ]
        
        return imageRep.representation(using: .jpeg, properties: options)
    }
    #endif
}

// MARK: - Label
#if os(iOS)
typealias AGLabel = UILabel
#else
typealias AGLabel = NSTextField
#endif
extension AGLabel {
    var formattedFloatValue: Float {
        get {
        #if os(iOS)
            if let text = text, let value = Double(text) {
                return Float(value)
            } else {
                return 0
            }
        #else
            return floatValue
        #endif
        }
        set {
        #if os(iOS)
            text = NSString(format: "%.1f", newValue) as String
        #else
            stringValue = NSString(format: "%.1f", newValue) as String
        #endif
        }
    }
    
    var formattedCGFloatValue: CGFloat {
        get {
        #if os(iOS)
            if let text = text, let value = Double(text) {
                return CGFloat(value)
            } else {
                return 0
            }
        #else
            return CGFloat(floatValue)
        #endif
        }
        set {
        #if os(iOS)
            text = NSString(format: "%.1f", newValue) as String
        #else
            stringValue = NSString(format: "%.1f", newValue) as String
        #endif
        }
    }
    
    var formattedIntValue: Int {
        get {
            #if os(iOS)
                if let text = text, let value = Int(text) {
                    return value
                } else {
                    return 0
                }
            #else
                return integerValue
            #endif
        }
        set {
            #if os(iOS)
                text = "\(newValue)"
            #else
                stringValue = "\(newValue)"
            #endif
        }
    }
    
#if os(macOS)
    var text: String? {
        get {
            return stringValue
        }
        set {
            if let newValue = newValue {
                stringValue = newValue
            }
        }
    }
#endif
}

//MARK: - TextField
#if os(iOS)
typealias AGTextField = UITextField
#else
typealias AGTextField = NSTextField
#endif
#if os(iOS)
extension AGTextField {
    var integerValue: Int {
        get {
            if let text = text, let value = Int(text) {
                return value
            } else {
                return 0
            }
        }
        set {
            text = "\(newValue)"
        }
    }
    
    var formattedIntValue: Int {
        get {
            return integerValue
        }
        set {
            integerValue = newValue
        }
    }
    
    var cgFloatValue: CGFloat {
        get {
            if let text = text, let value = Double(text) {
                return CGFloat(value)
            } else {
                return 0
            }
        }
        set {
            text = "\(newValue)"
        }
    }
    
    var formattedCGFloatValue: CGFloat {
        get {
            return CGFloat(cgFloatValue)
        }
        set {
            cgFloatValue = newValue
        }
    }
    
    var formattedFloatValue: Float {
        get {
            if let text = text, let value = Double(text) {
                return Float(value)
            } else {
                return 0
            }
        }
        set {
            text = NSString(format: "%.1f", newValue) as String
        }
    }
    
    var stringValue: String {
        get {
            return text!
        }
        set {
            text = newValue
        }
    }
}
#endif

//MARK: - Indicator
#if os(iOS)
typealias AGIndicator = UIActivityIndicatorView
#else
typealias AGIndicator = NSProgressIndicator
#endif

//MARK: - View
#if os(iOS)
public typealias AGView = UIView
#else
public typealias AGView = NSView
#endif
extension AGView {
#if os(macOS)
    var backgroundColor: AGColor? {
        get {
            if let cgColor = layer?.backgroundColor {
                return AGColor(cgColor: cgColor)
            } else {
                return nil
            }
        }
        set {
            if let newValue = newValue {
                wantsLayer = true
                layer?.backgroundColor = newValue.cgColor
            }
        }
    }
#endif
}


//MARK: - ImageView
#if os(iOS)
typealias AGImageView = UIImageView
#else
typealias AGImageView = NSImageView
#endif

//MARK: - TableView
#if os(iOS)
typealias AGTableView = UITableView
#else
typealias AGTableView = NSTableView
#endif

//MARK: - TableViewCell
#if os(iOS)
typealias AGTableViewCell = UITableViewCell
#else
typealias AGTableViewCell = NSTableCellView
#endif

//MARK: - Button
#if os(iOS)
typealias AGButton = UIButton
#else
typealias AGButton = NSButton
#endif
extension AGButton {
#if os(iOS)
    var image: AGImage? {
        get {
            return image(for: .normal)
        }
        set {
            setImage(newValue, for: .normal)
        }
    }
    var highlightImage: AGImage? {
        get {
            return image(for: .highlighted)
        }
        set {
            setImage(newValue, for: .highlighted)
        }
    }
    var title: String? {
        get {
            return title(for: .normal)
        }
        set {
            setTitle(newValue, for: .normal)
        }
    }
#else
    var textColor: AGColor {
        get {
            return AGColor.black
        }
        set {
            let pstyle = NSMutableParagraphStyle()
            pstyle.alignment = .left
            attributedTitle = NSAttributedString(string: title, attributes: [ NSAttributedString.Key.foregroundColor : newValue, NSAttributedString.Key.paragraphStyle : pstyle ])
        }
    }
#endif
}

//MARK: - Switch
#if os(iOS)
typealias AGSwitch = UISwitch
#else
typealias AGSwitch = NSButton
#endif
#if os(macOS)
extension AGSwitch {
    var isOn: Bool {
        get {
            return state != .off
        }
        set {
            state = newValue ? .on : .off
        }
    }
}
#endif

//MARK: - Slider
#if os(iOS)
typealias AGSlider = UISlider
#else
typealias AGSlider = NSSlider
#endif
extension AGSlider {
#if os(iOS)
    var floatValue: Float {
        get {
            return value
        }
        set {
            setValue(newValue, animated: false)
        }
    }
    var cgFloatValue: CGFloat {
        get {
            return CGFloat(value)
        }
        set {
            setValue(Float(newValue), animated: false)
        }
    }
    var integerValue: Int {
        get {
            return Int(value)
        }
        set {
            setValue(Float(newValue), animated: false)
        }
    }
    var doubleValue: Double {
        get {
            return Double(value)
        }
        set {
            setValue(Float(newValue), animated: false)
        }
    }
#else
    var minimumValue: Float {
        get {
            return Float(minValue)
        }
        set {
            minValue = Double(newValue)
        }
    }
    var maximumValue: Float {
        get {
            return Float(maxValue)
        }
        set {
            maxValue = Double(newValue)
        }
    }
#endif
}

//MARK: - SegmentedControl
#if os(iOS)
typealias AGPopSheetButton = UIButton
#else
typealias AGPopSheetButton = NSPopUpButton
#endif

//MARK: - SegmentedControl
#if os(iOS)
typealias AGSegmentedControl = UISegmentedControl
#else
typealias AGSegmentedControl = NSPopUpButton
#endif
#if os(macOS)
extension AGSegmentedControl {
    var selectedSegmentIndex: Int {
        get {
            return indexOfSelectedItem
        }
        set {
            selectItem(at: newValue)
        }
    }
}
#endif

//MARK: - StoryboardSegue
#if os(iOS)
typealias AGStoryboardSegue = UIStoryboardSegue
#else
typealias AGStoryboardSegue = NSStoryboardSegue
#endif
extension AGStoryboardSegue {
    var identifierString: String? {
        get {
        #if os(iOS)
            return identifier
        #else
            return identifier
        #endif
        }
    }
    
#if os(iOS)
    var destinationController: AGViewController? {
        get {
            return destination
        }
    }
#endif
}

//MARK: - ViewController
#if os(iOS)
typealias AGViewController = UIViewController
#else
typealias AGViewController = NSViewController
#endif
extension AGViewController {
    func performAGSegue(withIdentifier identifier: String, sender: Any?) {
    #if os(iOS)
        performSegue(withIdentifier: identifier, sender: sender)
    #else
        performSegue(withIdentifier: identifier, sender: sender)
    #endif
    }
    
    func dismissVC(_ vc: AGViewController, animated: Bool) {
    #if os(iOS)
        vc.dismiss(animated: animated, completion: nil)
    #else
        dismiss(vc)
    #endif
    }
}

//MARK: - TableViewController
#if os(iOS)
typealias AGTableViewController = UITableViewController
#else
typealias AGTableViewController = NSViewController
#endif
