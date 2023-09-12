extends GDCubismEffect
class_name GDCubismCustomEffectLipSync1


const MIN_DB: float = 60
const MIN_VOICE_HZ: int = 0
const MAX_VOICE_HZ: int = 3600
const ary_map: Array = [
    ["ParamMouthA", "a"]
]
const HISTORY_SIZE: int = 10

var spectrum: AudioEffectSpectrumAnalyzerInstance
var dict_mouth: Dictionary
var ary_formant_history: Array
var result: String
var lipsync_ready: bool = false


class AvgValue:
    var v: float
    var hz: float

    func _init(v: float, hz: float):
        self.v = v
        self.hz = hz


func _cubism_init(model: GDCubismUserModel):
    dict_mouth.clear()
    ary_formant_history.clear()

    var ary_param = model.get_parameters()

    for param in ary_param:
        for map in ary_map:
            if param.id == map[0]:
                dict_mouth[map[1]] = param

    lipsync_ready = true


func _cubism_term(model: GDCubismUserModel):
    lipsync_ready = false


func _cubism_process(model: GDCubismUserModel, delta: float):

    if lipsync_ready == false:
        return

    var m: float = spectrum.get_magnitude_for_frequency_range(MIN_VOICE_HZ, MAX_VOICE_HZ).length()
    var v = clamp((MIN_DB + linear_to_db(m)) / MIN_DB, 0.0, 1.0)


    if (v) < 0.1:
        result = ""
    else:
        result = "a"

    if dict_mouth.has(result) == true:
        dict_mouth[result].value = 1.0

    for k in dict_mouth.keys():
        dict_mouth[k].value = max(dict_mouth[k].value - 0.2, 0.0)

