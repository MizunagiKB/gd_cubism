extends GDCubismEffect
class_name GDCubismCustomEffectLipSync2


const MIN_DB: float = 60
const MIN_VOICE_HZ: int = 0
const MAX_VOICE_HZ: int = 3600
const SLICE_HZ: int = 100
const ary_map: Array = [
    ["ParamMouthA", "a"],
    ["ParamMouthI", "i"],
    ["ParamMouthU", "u"],
    ["ParamMouthE", "e"],
    ["ParamMouthO", "o"]
]
const HISTORY_SIZE: int = 6

var spectrum: AudioEffectSpectrumAnalyzerInstance
var dict_mouth: Dictionary
var ary_formant_history: Array
var history_pos: int = 0

var f1_hz: float
var f1_v: float
var f2_hz: float
var f2_v: float
var result: String
var lipsync_ready: bool = false


class AvgValue:
    var v: float
    var hz: float

    func _init(v: float, hz: float):
        self.v = v
        self.hz = hz


func cmp_value(a: AvgValue, b: AvgValue):
    if a.v > b.v:
        return true
    return false


func formant(f1: float, f2: float) -> String:

    if f1_hz > 600 and f1_hz < 1400:
        if f2_hz > 900 and f2_hz < 2000:
            return "a"

    if f1_hz > 100 and f1_hz < 500:
        if f2_hz > 1800 and f2_hz < 3600:
            return "i"

    if f1_hz > 100 and f1_hz < 800:
        if f2_hz > 600 and f2_hz < 2000:
            return "u"

    if f1_hz > 400 and f1_hz < 800:
        if f2_hz > 1600 and f2_hz < 3600:
            return "e"

    if f1_hz > 200 and f1_hz < 1000:
        if f2_hz > 400 and f2_hz < 1500:
            return "o"

    return result


func formant_calc():
    if lipsync_ready == false:
        return

    for i in range(MAX_VOICE_HZ / SLICE_HZ):
        var hz: float = SLICE_HZ * i
        var m: float = spectrum.get_magnitude_for_frequency_range(hz, hz + SLICE_HZ).length()
        var e = clamp((MIN_DB + linear_to_db(m)) / MIN_DB, 0.0, 1.0)

        ary_formant_history[i][history_pos] = e

    history_pos += 1
    if history_pos >= HISTORY_SIZE:
        history_pos = 0

    var hz: float = float(SLICE_HZ) / 2.0
    var ary_avg: Array
    for ary_history in ary_formant_history:
        var sum_v: float = 0.0
        for v in ary_history:
            sum_v += v
        hz += SLICE_HZ
        ary_avg.append(AvgValue.new(sum_v / HISTORY_SIZE, hz))

    ary_avg.sort_custom(cmp_value)
    f1_v = ary_avg[0].v
    f1_hz = ary_avg[0].hz
    f2_v = ary_avg[1].v
    f2_hz = ary_avg[1].hz

    if (f1_v + f2_v) < 0.1:
        f1_hz = 0.0
        f2_hz = 0.0
        result = ""
    else:
        result = formant(f1_hz, f2_hz)

    if dict_mouth.has(result) == true:
        dict_mouth[result].value = 1.0


func _cubism_init(model: GDCubismUserModel):
    dict_mouth.clear()
    ary_formant_history.clear()

    var ary_param = model.get_parameters()

    for param in ary_param:
        for map in ary_map:
            if param.id == map[0]:
                dict_mouth[map[1]] = param

    for i in range(MAX_VOICE_HZ / SLICE_HZ):
        var ary_history: Array
        for j in range(HISTORY_SIZE):
            ary_history.append(0.0)
        ary_formant_history.append(ary_history)

    lipsync_ready = true


func _cubism_term(model: GDCubismUserModel):
    lipsync_ready = false


func _cubism_process(model: GDCubismUserModel, delta: float):

    formant_calc()

    for k in dict_mouth.keys():
        dict_mouth[k].value = max(dict_mouth[k].value - 0.2, 0.0)

